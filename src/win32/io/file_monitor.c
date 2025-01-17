// #include <xcl/io/file_monitor.h>
// #include <windows.h>
// #include <xcl/concurrent/mutex.h>
// #include <xcl/io/file.h>
//
// #include <stdio.h>
// #include <assert.h>
//
// typedef enum MonitorParam
//{
//     RECURSIVE = 0,
//     AUTO_MONITOR = 1,
//     MANUAL_MONITOR = 2,
//     ASYNC_REQ_QUEUED = 8
// } MonitorParam;
//
// struct FileMonitor {
//     _aligned(4) volatile int32_t params;
//     HANDLE dirHandle;
//     HANDLE cancelHandle;
//     OVERLAPPED ovl;
//     _aligned(4) char cache[64 << 10];
//     void* mutex;
// };
//
// static inline bool __FileMonitor_isParamBitSet(FileMonitor* monitor,
//                                                int32_t idx)
//{
//     int32_t params = InterlockedOr(&monitor->params, 0);
//     return params & (1 << idx);
// }
//
// static inline void
//__FileMonitor_setParamBit(FileMonitor* monitor, int32_t idx, bool flag)
//{
//     int32_t params = InterlockedOr(&monitor->params, 0);
//     InterlockedExchange(&monitor->params,
//                         flag ? (params | (1 << idx)) : (params & ~(1 <<
//                         idx)));
// }
//
// bool  FileMonitor_exclusive(FileMonitor* monitor)
//{
//     return __FileMonitor_isParamBitSet(monitor, AUTO_MONITOR)
//            || __FileMonitor_isParamBitSet(monitor, MANUAL_MONITOR);
// }
//
// FileMonitor*  FileMonitor_new(const char* path, bool recursive)
//{
//     DWORD err = GetLastError();
//     if (File_isDir(path) != 1)
//     {
//         return NULL;
//     }
//     FileMonitor* monitor = malloc(sizeof(FileMonitor));
//     if (!monitor)
//     {
//         return NULL;
//     }
//     memset(monitor, 0, sizeof(FileMonitor));
//     if ((monitor->ovl.hEvent = CreateEvent(NULL, true, false, NULL))
//         && (monitor->cancelHandle = CreateEvent(NULL, false, false, NULL))
//         && (monitor->mutex = Mutex_new())
//         && (monitor->dirHandle
//             = CreateFile(path,
//                          FILE_LIST_DIRECTORY,
//                          FILE_SHARE_DELETE | FILE_SHARE_READ |
//                          FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
//                          FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
//                          NULL))
//                != INVALID_HANDLE_VALUE)
//     {
//         __FileMonitor_setParamBit(monitor, RECURSIVE, recursive);
//         return monitor;
//     }
//     if (err != GetLastError())
//     {
//     }
//     if (monitor->cancelHandle)
//     {
//         CloseHandle(monitor->cancelHandle);
//     }
//     if (monitor->mutex)
//     {
//         Mutex_delete(monitor->mutex);
//     }
//     if (monitor->dirHandle != INVALID_HANDLE_VALUE)
//     {
//         CloseHandle(monitor->dirHandle);
//     }
//     if (monitor->ovl.hEvent)
//     {
//         CloseHandle(monitor->ovl.hEvent);
//     }
//     free(monitor);
//     return NULL;
// }
//
// static int32_t __FileMonitor_processChanges2(FileMonitor* monitor,
//                                              DWORD millis,
//                                              MonitorCallback callback,
//                                              void* bq)
//{
//     int32_t ret = 1;
//     DWORD bytes;
//     if ((millis == INFINITE ? GetOverlappedResult(monitor->dirHandle,
//                                                   &monitor->ovl,
//                                                   &bytes,
//                                                   true)
//          : millis == 0      ? GetOverlappedResult(monitor->dirHandle,
//                                              &monitor->ovl,
//                                              &bytes,
//                                              false)
//                             : GetOverlappedResultEx(monitor->dirHandle,
//                                                &monitor->ovl,
//                                                &bytes,
//                                                millis,
//                                                false)))
//     {
//         assert(bytes > 0);
//         __FileMonitor_setParamBit(monitor, ASYNC_REQ_QUEUED, false);
//         ResetEvent(monitor->ovl.hEvent);
//         FILE_NOTIFY_INFORMATION* fni =
//         (FILE_NOTIFY_INFORMATION*)monitor->cache; for (;;)
//         {
//             if (fni->Action)
//             {
//                 FileAction action = 0;
//                 if (fni->Action >= 4)
//                 {
//                     action = FILE_RENAME;
//                 }
//                 else
//                 {
//                     action = fni->Action;
//                 }
//                 callback(bq,
//                          action,
//                          fni->FileName,
//                          fni->FileNameLength / sizeof(wchar_t));
//             }
//             if (fni->NextEntryOffset == 0)
//             {
//                 break;
//             }
//             fni = (FILE_NOTIFY_INFORMATION*)((char*)fni +
//             fni->NextEntryOffset);
//         }
//     }
//     else
//     {
//         ret = -1;
//         DWORD err = GetLastError();
//         static DWORD __asyncIoErr[] = {ERROR_IO_PENDING,
//                                        ERROR_IO_INCOMPLETE,
//                                        WAIT_IO_COMPLETION,
//                                        WAIT_TIMEOUT};
//         if (millis != INFINITE)
//         {
//             for (int i = 0; i < sizeof(__asyncIoErr) / sizeof(DWORD); i++)
//             {
//                 if (__asyncIoErr[i] == err)
//                 {
//                     ret = 0;
//                     break;
//                 }
//             }
//         }
//         if (ret == -1)
//         {
//         }
//     }
//     return ret;
// }
//
// static int32_t __FileMonitor_processChanges(FileMonitor* monitor,
//                                             bool wait,
//                                             MonitorCallback callback,
//                                             void* bq)
//{
//     return __FileMonitor_processChanges2(monitor,
//                                          wait ? INFINITE : 0,
//                                          callback,
//                                          bq);
// }
//
// static bool __FileMonitor_postListenReq(FileMonitor* monitor)
//{
//     if (!__FileMonitor_isParamBitSet(monitor, ASYNC_REQ_QUEUED))
//     {
//         bool queued = ReadDirectoryChangesW(
//             monitor->dirHandle,
//             monitor->cache,
//             sizeof(monitor->cache),
//             __FileMonitor_isParamBitSet(monitor, RECURSIVE),
//             FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES
//                 | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_DIR_NAME
//                 | FILE_NOTIFY_CHANGE_FILE_NAME |
//                 FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_SECURITY
//                 | FILE_NOTIFY_CHANGE_SIZE,
//             NULL,
//             &monitor->ovl,
//             NULL);
//         if (!queued)
//         {
//
//             return false;
//         }
//         __FileMonitor_setParamBit(monitor, ASYNC_REQ_QUEUED, true);
//     }
//     return true;
// }
//
// bool
// FileMonitor_exec(FileMonitor* monitor, MonitorCallback callback, void* bq)
//{
//     Mutex_lock(monitor->mutex);
//     if (FileMonitor_exclusive(monitor))
//     {
//         Mutex_lock(monitor->mutex);
//         return false;
//     }
//     __FileMonitor_setParamBit(monitor, AUTO_MONITOR, true);
//     bool success = true;
//     assert(__FileMonitor_isParamBitSet(monitor, ASYNC_REQ_QUEUED) == false);
//     DWORD bytes;
//     HANDLE handles[2] = {monitor->ovl.hEvent, monitor->cancelHandle};
//     bool cancelled = false;
//     while (success && !cancelled)
//     {
//         if (!__FileMonitor_postListenReq(monitor))
//         {
//             success = false;
//             break;
//         }
//         Mutex_unlock(monitor->mutex);
//         DWORD waitRet = WaitForMultipleObjects(2, handles, false, INFINITE);
//         Mutex_lock(monitor->mutex);
//         switch (waitRet)
//         {
//         case WAIT_OBJECT_0: {
//             int32_t ret
//                 = __FileMonitor_processChanges(monitor, true, callback, bq);
//             assert(ret != -1);
//             break;
//         }
//         case WAIT_OBJECT_0 + 1: {
//             /**
//              * @brief detect cancel event
//              * @author xuyan
//              * @date 2022-08-20
//              */
//             cancelled = true;
//             CancelIo(monitor->dirHandle);
//             __FileMonitor_processChanges(monitor, true, callback, bq);
//             __FileMonitor_setParamBit(monitor, ASYNC_REQ_QUEUED, false);
//             break;
//         }
//         default: {
//             assert(false);
//         }
//         }
//     }
//     __FileMonitor_setParamBit(monitor, AUTO_MONITOR, false);
//     Mutex_unlock(monitor->mutex);
//     return success;
// }
//
// bool  FileMonitor_cancel(FileMonitor* monitor)
//{
//     bool ret = true;
//     SetEvent(monitor->cancelHandle);
//     return ret;
// }
//
// void  FileMonitor_delete(FileMonitor* monitor)
//{
//     Mutex_delete(monitor->mutex);
//     CloseHandle(monitor->dirHandle);
//     CloseHandle(monitor->ovl.hEvent);
//     CloseHandle(monitor->cancelHandle);
//     free(monitor);
// }
//
// bool  FileMonitor_listen(FileMonitor* monitor,
//                                    uint32_t millis,
//                                    MonitorCallback callback,
//                                    void* bq)
//{
//     Mutex_lock(monitor->mutex);
//     bool success = true;
//     if (FileMonitor_exclusive(monitor))
//     {
//         Mutex_lock(monitor->mutex);
//         return false;
//     }
//     __FileMonitor_setParamBit(monitor, MANUAL_MONITOR, true);
//     success = __FileMonitor_postListenReq(monitor);
//     if (success)
//     {
//         HANDLE handles[] = {monitor->ovl.hEvent, monitor->cancelHandle};
//         DWORD waitRet = WaitForMultipleObjects(2, handles, false, millis);
//         switch (waitRet)
//         {
//         case WAIT_TIMEOUT: {
//             success = false;
//             break;
//         }
//         case WAIT_OBJECT_0: {
//             int32_t ret;
//             ret = __FileMonitor_processChanges2(monitor, millis, callback,
//             bq); assert(ret != -1); break;
//         }
//         case WAIT_OBJECT_0 + 1: {
//             /**
//              * @brief cancel event detected
//              * @author xuyan
//              * @date 2022-08-21
//              */
//             CancelIo(monitor->dirHandle);
//             __FileMonitor_processChanges(monitor, true, callback, bq);
//             __FileMonitor_setParamBit(monitor, ASYNC_REQ_QUEUED, false);
//             success = false;
//             break;
//         }
//         default: {
//             assert(false);
//         }
//         }
//     }
//     __FileMonitor_setParamBit(monitor, MANUAL_MONITOR, false);
//     Mutex_unlock(monitor->mutex);
//     return success;
// }
