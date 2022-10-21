#include <xcl/io/file_monitor.h>
#include <windows.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/concurrent/xcl_atomic.h>
#include <xcl/io/file.h>
#include <xcl/lang/xcl_err.h>
#include <xcl/lang/inter_thread_api.h>
#include <stdio.h>
#include <assert.h>

typedef enum MonitorParam
{
    RECURSIVE = 0,
    AUTO_MONITOR = 1,
    MANUAL_MONITOR = 2,
    ASYNC_REQ_QUEUED = 8
} MonitorParam;

struct FileMonitor {
    ALIGNED(4) ATOMIC(int32_t) params;
    HANDLE dir_handle;
    HANDLE cancel_handle;
    OVERLAPPED ovl;
    ALIGNED(4) char cache[64 << 10];
    Mutex* mutex;
};

static inline bool
__FileMonitor_isParamBitSet(FileMonitor* monitor, int32_t idx)
{
    int32_t params = __Atomic_load32(&monitor->params, memory_order_seq_cst);
    return params & (1 << idx);
}

static inline void
__FileMonitor_setParamBit(FileMonitor* monitor, int32_t idx, bool flag)
{
    int32_t params = __Atomic_load32(&monitor->params, memory_order_seq_cst);
    __Atomic_store32(&monitor->params,
                     flag ? (params | (1 << idx)) : (params & ~(1 << idx)),
                     memory_order_seq_cst);
}

XCL_EXPORT bool XCL_API
FileMonitor_exclusive(FileMonitor* monitor)
{
    return __FileMonitor_isParamBitSet(monitor, AUTO_MONITOR)
           || __FileMonitor_isParamBitSet(monitor, MANUAL_MONITOR);
}

XCL_EXPORT FileMonitor* XCL_API
FileMonitor_new(const TChar* path, bool recursive)
{
    DWORD err = GetLastError();
    if (!File_isPathDir(path))
    {
        return NULL;
    }
    FileMonitor* monitor = malloc(sizeof(FileMonitor));
    if (!monitor)
    {
        return NULL;
    }
    memset(monitor, 0, sizeof(FileMonitor));
    if ((monitor->ovl.hEvent = CreateEvent(NULL, true, false, NULL))
        && (monitor->cancel_handle = CreateEvent(NULL, false, false, NULL))
        && (monitor->mutex = Mutex_new())
        && (monitor->dir_handle
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
    if (monitor->cancel_handle)
    {
        CloseHandle(monitor->cancel_handle);
    }
    if (monitor->mutex)
    {
        Mutex_delete(monitor->mutex);
    }
    if (monitor->dir_handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(monitor->dir_handle);
    }
    if (monitor->ovl.hEvent)
    {
        CloseHandle(monitor->ovl.hEvent);
    }
    free(monitor);
    return NULL;
}

static int32_t
__FileMonitor_processChanges2(FileMonitor* monitor,
                              DWORD millis,
                              MonitorCallback callback,
                              void* obj)
{
    int32_t ret = 1;
    DWORD bytes;
    if ((millis == INFINITE ? GetOverlappedResult(monitor->dir_handle,
                                                  &monitor->ovl,
                                                  &bytes,
                                                  true)
         : millis == 0      ? GetOverlappedResult(monitor->dir_handle,
                                             &monitor->ovl,
                                             &bytes,
                                             false)
                            : GetOverlappedResultEx(monitor->dir_handle,
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
                FileAction action = 0;
                if (fni->Action >= 4)
                {
                    action = FILE_RENAME;
                }
                else
                {
                    action = fni->Action;
                }
                callback(obj,
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
        static DWORD __async_io_err[] = {ERROR_IO_PENDING,
                                         ERROR_IO_INCOMPLETE,
                                         WAIT_IO_COMPLETION,
                                         WAIT_TIMEOUT};
        if (millis != INFINITE)
        {
            for (int i = 0; i < sizeof(__async_io_err) / sizeof(DWORD); i++)
            {
                if (__async_io_err[i] == err)
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
__FileMonitor_processChanges(FileMonitor* monitor,
                             bool wait,
                             MonitorCallback callback,
                             void* obj)
{
    return __FileMonitor_processChanges2(monitor,
                                         wait ? INFINITE : 0,
                                         callback,
                                         obj);
}

static bool
__FileMonitor_postListenReq(FileMonitor* monitor)
{
    if (!__FileMonitor_isParamBitSet(monitor, ASYNC_REQ_QUEUED))
    {
        bool queued = ReadDirectoryChangesW(
            monitor->dir_handle,
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
FileMonitor_exec(FileMonitor* monitor, MonitorCallback callback, void* obj)
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
    HANDLE handles[2] = {monitor->ovl.hEvent, monitor->cancel_handle};
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
                int32_t ret = __FileMonitor_processChanges(monitor,
                                                           true,
                                                           callback,
                                                           obj);
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
                CancelIo(monitor->dir_handle);
                __FileMonitor_processChanges(monitor, true, callback, obj);
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
FileMonitor_cancel(FileMonitor* monitor)
{
    bool ret = true;
    SetEvent(monitor->cancel_handle);
    return ret;
}

XCL_EXPORT void XCL_API
FileMonitor_delete(FileMonitor* monitor)
{
    Mutex_delete(monitor->mutex);
    CloseHandle(monitor->dir_handle);
    CloseHandle(monitor->ovl.hEvent);
    CloseHandle(monitor->cancel_handle);
    free(monitor);
}

XCL_EXPORT bool XCL_API
FileMonitor_listen(FileMonitor* monitor,
                   uint32_t millis,
                   MonitorCallback callback,
                   void* obj)
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
        HANDLE handles[] = {monitor->ovl.hEvent, monitor->cancel_handle};
        DWORD waitRet = WaitForMultipleObjects(2, handles, false, millis);
        switch (waitRet)
        {
            case WAIT_TIMEOUT: {
                success = false;
                break;
            }
            case WAIT_OBJECT_0: {
                int32_t ret;
                ret = __FileMonitor_processChanges2(monitor,
                                                    millis,
                                                    callback,
                                                    obj);
                assert(ret != -1);
                break;
            }
            case WAIT_OBJECT_0 + 1: {
                /**
                 * @brief cancel event detected
                 * @author xuyan
                 * @date 2022-08-21
                 */
                CancelIo(monitor->dir_handle);
                __FileMonitor_processChanges(monitor, true, callback, obj);
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
