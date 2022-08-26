#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/util/Str.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/XclDef.h>

typedef struct _CFileMonitor_st CFileMonitor;

typedef enum
{
    FILE_ADDED = 1,
    FILE_REMOVED,
    FILE_MODIFY,
    FILE_RENAME
} CFileAction;

typedef void (*MonitorCallback)(void*,
                                CFileAction action,
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
XCL_EXPORT CFileMonitor* XCL_API
FileMonitor_new(const __FilePathChr* path, bool recursive);

/**
 * @brief listen file changes of monitor path, block util
 * another thread call FileMonitor_cancel
 * @param monitor file monitor object
 * @param cb callback to handle changed files
 * @param usr user defined object for callback
 * @return true if no error occured, false otherwise
 * @author xuyan
 * @date 2022-08-20
 */
XCL_EXPORT bool XCL_API
FileMonitor_exec(CFileMonitor* monitor, MonitorCallback cb, void* usr);

/**
 * @brief only exec one listen progress util timeout or cancelled
 * @param monitor file monitor object
 * @param millis timeout of milliseconds
 * @param cb directory changes callback
 * @param usr user specified object for callback
 * @return true if changes occurred, false if operation cancelled or timeout
 * @author xuyan
 * @date 2022-08-21
 */
XCL_EXPORT bool XCL_API
FileMonitor_listen(CFileMonitor* monitor,
                   uint32_t millis,
                   MonitorCallback cb,
                   void* usr);

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
FileMonitor_cancel(CFileMonitor* monitor);

/**
 * @brief delete file monitor object
 * this function should invoked when all listen progresses
 * are all done.
 * @param monitor file monitor object
 * @author xuyan
 * @date 2022-08-20
 */
XCL_EXPORT void XCL_API
FileMonitor_delete(CFileMonitor* monitor);

/**
 * @brief get is passing monitor is monitoring path
 * @param monitor file monitor object
 * @return true if monitor is working, false otherwise
 * @author xuyan
 * @date 2022-08-21
 */
XCL_EXPORT bool XCL_API
FileMonitor_exclusive(CFileMonitor* monitor);

#ifdef __cplusplus
}
#endif