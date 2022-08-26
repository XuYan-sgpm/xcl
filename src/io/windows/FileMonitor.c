#include <xcl/io/FileMonitor.h>
#include <windows.h>
#include <xcl/concurrent/Mutex.h>
#include <xcl/concurrent/XclAtomic.h>
#include <xcl/io/File.h>
#include <xcl/lang/XclErr.h>
#include <xcl/lang/InterThreadApi.h>
#include <stdio.h>
#include <assert.h>

typedef enum
{
    RECURSIVE = 0,
    AUTO_MONITOR = 1,
    MANUAL_MONITOR = 2,
    ASYNC_REQ_QUEUED = 8
} CMonitorParam;

struct _CFileMonitor_st {
    ALIGNED(4) ATOMIC(int32_t) params;
    HANDLE dirHandle;
    HANDLE cancelHandle;
    OVERLAPPED ovl;
    ALIGNED(4) char cache[64 << 10];
    CMutex* mutex;
};

static inline bool
__FileMonitor_isParamBitSet(CFileMonitor* monitor, int32_t idx)
{
    int32_t params = __Atomic_load32(&monitor->params, memory_order_seq_cst);
    return params & (1 << idx);
}

static inline void
__FileMonitor_setParamBit(CFileMonitor* monitor, int32_t idx, bool flag)
{
    int32_t params = __Atomic_load32(&monitor->params, memory_order_seq_cst);
    __Atomic_store32(&monitor->params,
                     flag ? (params | (1 << idx)) : (params & ~(1 << idx)),
                     memory_order_seq_cst);
}

XCL_EXPORT bool XCL_API
FileMonitor_exclusive(CFileMonitor* monitor)
{
    return __FileMonitor_isParamBitSet(monitor, AUTO_MONITOR)
           || __FileMonitor_isParamBitSet(monitor, MANUAL_MONITOR);
}

XCL_EXPORT CFileMonitor* XCL_API
FileMonitor_new(const __FilePathChr* path, bool recursive)
{
    DWORD err = GetLastError();
    if (!File_isPathDir(path))
    {
        return NULL;
    }
    CFileMonitor* monitor = malloc(sizeof(CFileMonitor));
    if (!monitor)
    {
        return NULL;
    }
    memset(monitor, 0, sizeof(CFileMonitor));
    if ((monitor->ovl.hEvent = CreateEvent(NULL, true, false, NULL))
        && (monitor->cancelHandle = CreateEvent(NULL, false, false, NULL))
        && (monitor->mutex = Mutex_new())
        && (monitor->dirHandle
            = CreateFile(path,
                         FILE_LIST_DIRECTORY,
                         FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                         NULL,
                         OPEN_EXISTING,
                         FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                         NULL))
               != INVALID_HANDLE_VALUE)
    {
        __FileMonitor_setParamBit(monitor, RECURSIVE, recursive);
        return monitor;
    }
    if (err != GetLastError())
    {
        Err_set(GetLastError());
    }
    if (monitor->cancelHandle)
    {
        CloseHandle(monitor->cancelHandle);
    }
    if (monitor->mutex)
    {
        Mutex_delete(monitor->mutex);
    }
    if (monitor->dirHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(monitor->dirHandle);
    }
    if (monitor->ovl.hEvent)
    {
        CloseHandle(monitor->ovl.hEvent);
    }
    free(monitor);
    return NULL;
}

static int32_t
__FileMonitor_processChanges2(CFileMonitor* monitor,
                              DWORD millis,
                              MonitorCallback cb,
                              void* usr)
{
    int32_t ret = 1;
    DWORD bytes;
    if ((millis == INFINITE ? GetOverlappedResult(monitor->dirHandle,
                                                  &monitor->ovl,
                                                  &bytes,
                                                  true)
         : millis == 0      ? GetOverlappedResult(monitor->dirHandle,
                                             &monitor->ovl,
                                             &bytes,
                                             false)
                            : GetOverlappedResultEx(monitor->dirHandle,
                                               &monitor->ovl,
                                               &bytes,
                                               millis,
                                               false)))
    {
        assert(bytes > 0);
        __FileMonitor_setParamBit(monitor, ASYNC_REQ_QUEUED, false);
        ResetEvent(monitor->ovl.hEvent);
        FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)monitor->cache;
        for (;;)
        {
            if (fni->Action)
            {
                CFileAction action = 0;
                if (fni->Action >= 4)
                {
                    action = FILE_RENAME;
                }
                else
                {
                    action = fni->Action;
                }
                cb(usr,
                   action,
                   fni->FileName,
                   fni->FileNameLength / sizeof(wchar_t));
            }
            if (fni->NextEntryOffset == 0)
            {
                break;
            }
            fni = (FILE_NOTIFY_INFORMATION*)((char*)fni + fni->NextEntryOffset);
        }
    }
    else
    {
        ret = -1;
        DWORD err = GetLastError();
        static DWORD __asyncIoErr[] = {ERROR_IO_PENDING,
                                       ERROR_IO_INCOMPLETE,
                                       WAIT_IO_COMPLETION,
                                       WAIT_TIMEOUT};
        if (millis != INFINITE)
        {
            for (int i = 0; i < sizeof(__asyncIoErr) / sizeof(DWORD); i++)
            {
                if (__asyncIoErr[i] == err)
                {
                    ret = 0;
                    break;
                }
            }
        }
        if (ret == -1)
        {
            Err_set(err);
        }
    }
    return ret;
}

static int32_t
__FileMonitor_processChanges(CFileMonitor* monitor,
                             bool wait,
                             MonitorCallback cb,
                             void* usr)
{
    return __FileMonitor_processChanges2(monitor, wait ? INFINITE : 0, cb, usr);
}

static bool
__FileMonitor_postListenReq(CFileMonitor* monitor)
{
    if (!__FileMonitor_isParamBitSet(monitor, ASYNC_REQ_QUEUED))
    {
        bool queued = ReadDirectoryChangesW(
            monitor->dirHandle,
            monitor->cache,
            sizeof(monitor->cache),
            __FileMonitor_isParamBitSet(monitor, RECURSIVE),
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES
                | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_DIR_NAME
                | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_ACCESS
                | FILE_NOTIFY_CHANGE_SECURITY | FILE_NOTIFY_CHANGE_SIZE,
            NULL,
            &monitor->ovl,
            NULL);
        if (!queued)
        {
            Err_set(GetLastError());
            return false;
        }
        __FileMonitor_setParamBit(monitor, ASYNC_REQ_QUEUED, true);
    }
    return true;
}

XCL_EXPORT bool XCL_API
FileMonitor_exec(CFileMonitor* monitor, MonitorCallback cb, void* usr)
{
    Mutex_lock(monitor->mutex);
    if (FileMonitor_exclusive(monitor))
    {
        Mutex_lock(monitor->mutex);
        return false;
    }
    __FileMonitor_setParamBit(monitor, AUTO_MONITOR, true);
    bool success = true;
    assert(__FileMonitor_isParamBitSet(monitor, ASYNC_REQ_QUEUED) == false);
    DWORD bytes;
    HANDLE handles[2] = {monitor->ovl.hEvent, monitor->cancelHandle};
    bool cancelled = false;
    while (success && !cancelled)
    {
        if (!__FileMonitor_postListenReq(monitor))
        {
            success = false;
            break;
        }
        Mutex_unlock(monitor->mutex);
        DWORD waitRet = WaitForMultipleObjects(2, handles, false, INFINITE);
        Mutex_lock(monitor->mutex);
        switch (waitRet)
        {
            case WAIT_OBJECT_0: {
                int32_t ret
                    = __FileMonitor_processChanges(monitor, true, cb, usr);
                assert(ret != -1);
                break;
            }
            case WAIT_OBJECT_0 + 1: {
                /**
                 * @brief detect cancel event
                 * @author xuyan
                 * @date 2022-08-20
                 */
                cancelled = true;
                CancelIo(monitor->dirHandle);
                __FileMonitor_processChanges(monitor, true, cb, usr);
                __FileMonitor_setParamBit(monitor, ASYNC_REQ_QUEUED, false);
                break;
            }
            default: {
                assert(false);
            }
        }
    }
    __FileMonitor_setParamBit(monitor, AUTO_MONITOR, false);
    Mutex_unlock(monitor->mutex);
    return success;
}

XCL_EXPORT bool XCL_API
FileMonitor_cancel(CFileMonitor* monitor)
{
    bool ret = true;
    SetEvent(monitor->cancelHandle);
    return ret;
}

XCL_EXPORT void XCL_API
FileMonitor_delete(CFileMonitor* monitor)
{
    Mutex_delete(monitor->mutex);
    CloseHandle(monitor->dirHandle);
    CloseHandle(monitor->ovl.hEvent);
    CloseHandle(monitor->cancelHandle);
    free(monitor);
}

XCL_EXPORT bool XCL_API
FileMonitor_listen(CFileMonitor* monitor,
                   uint32_t millis,
                   MonitorCallback cb,
                   void* usr)
{
    Mutex_lock(monitor->mutex);
    bool success = true;
    if (FileMonitor_exclusive(monitor))
    {
        Mutex_lock(monitor->mutex);
        return false;
    }
    __FileMonitor_setParamBit(monitor, MANUAL_MONITOR, true);
    success = __FileMonitor_postListenReq(monitor);
    if (success)
    {
        HANDLE handles[] = {monitor->ovl.hEvent, monitor->cancelHandle};
        DWORD waitRet = WaitForMultipleObjects(2, handles, false, millis);
        switch (waitRet)
        {
            case WAIT_TIMEOUT: {
                success = false;
                break;
            }
            case WAIT_OBJECT_0: {
                int32_t ret;
                ret = __FileMonitor_processChanges2(monitor, millis, cb, usr);
                assert(ret != -1);
                break;
            }
            case WAIT_OBJECT_0 + 1: {
                /**
                 * @brief cancel event detected
                 * @author xuyan
                 * @date 2022-08-21
                 */
                CancelIo(monitor->dirHandle);
                __FileMonitor_processChanges(monitor, true, cb, usr);
                __FileMonitor_setParamBit(monitor, ASYNC_REQ_QUEUED, false);
                success = false;
                break;
            }
            default: {
                assert(false);
            }
        }
    }
    __FileMonitor_setParamBit(monitor, MANUAL_MONITOR, false);
    Mutex_unlock(monitor->mutex);
    return success;
}
