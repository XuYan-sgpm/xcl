#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/util/str.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/xcl_def.h>

typedef struct FileMonitor FileMonitor;

typedef enum FileAction
{
    FILE_ADDED = 1,
    FILE_REMOVED,
    FILE_MODIFY,
    FILE_RENAME
} FileAction;

typedef void (*MonitorCallback)(void*,
                                FileAction action,
                                const wchar_t* name,
                                int32_t len);

/**
 * @brief create a file monitor
 * @param path file or directory path to monitor
 * @param recursive whether recursive monitor the
 * subtree of passing path
 * @return file monitor object
 * @author xuyan
 * @date 2022-08-20
 */
XCL_EXPORT FileMonitor* XCL_API
FileMonitor_new(const TChar* path, bool recursive);

/**
 * @brief listen file changes of monitor path, block util
 * another thread call FileMonitor_cancel
 * @param monitor file monitor object
 * @param cb callback to handle changed files
 * @param obj user defined object for callback
 * @return true if no error occured, false otherwise
 * @author xuyan
 * @date 2022-08-20
 */
XCL_EXPORT bool XCL_API
FileMonitor_exec(FileMonitor* monitor, MonitorCallback cb, void* obj);

/**
 * @brief only exec one listen progress util timeout or cancelled
 * @param monitor file monitor object
 * @param millis timeout of milliseconds
 * @param cb directory changes callback
 * @param obj user specified object for callback
 * @return true if changes occurred, false if operation cancelled or timeout
 * @author xuyan
 * @date 2022-08-21
 */
XCL_EXPORT bool XCL_API
FileMonitor_listen(FileMonitor* monitor,
                   uint32_t millis,
                   MonitorCallback cb,
                   void* obj);

/**
 * @brief cancel all block progress before
 * the block progress happened after is not affected
 * @param monitor file monitor object
 * @return true if cancel successfully, false otherwise
 * using Err_get to see error code
 * @author xuyan
 * @date 2022-08-20
 */
XCL_EXPORT bool XCL_API
FileMonitor_cancel(FileMonitor* monitor);

/**
 * @brief delete file monitor object
 * this function should invoked when all listen progresses
 * are all done.
 * @param monitor file monitor object
 * @author xuyan
 * @date 2022-08-20
 */
XCL_EXPORT void XCL_API
FileMonitor_delete(FileMonitor* monitor);

/**
 * @brief whether monitor is working
 * @param monitor file monitor object
 * @return true if monitor is working, false otherwise
 * @author xuyan
 * @date 2022-08-21
 */
XCL_EXPORT bool XCL_API
FileMonitor_exclusive(FileMonitor* monitor);

#ifdef __cplusplus
}
#endif