#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/lang/xcl_def.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcl/util/str.h>

typedef enum FileMode
{
    F_READ = 1,
    F_WRITE = 2,
    F_UPDATE = 4,
    F_APPEND = 8
} FileMode;

typedef struct File File;
typedef struct FileIter FileIter;

XCL_EXPORT File* XCL_API
File_new(const TChar* path, int32_t mode);

XCL_EXPORT bool XCL_API
File_isPathExists(const TChar* path);

XCL_EXPORT bool XCL_API
File_exists(const File* file);

XCL_EXPORT void XCL_API
File_delete(File* file);

XCL_EXPORT int32_t XCL_API
File_flush(File* file);

XCL_EXPORT int32_t XCL_API
File_read(File* file, void* dst, int32_t len);

XCL_EXPORT int32_t XCL_API
File_write(File* file, const void* src, int32_t len);

XCL_EXPORT bool XCL_API
File_remove(File* file);

XCL_EXPORT bool XCL_API
File_deletePath(const TChar* path);

XCL_EXPORT bool XCL_API
File_rename(File* file, const TChar* newFileName);

XCL_EXPORT bool XCL_API
File_renamePath(const TChar* old, const TChar* now);

XCL_EXPORT bool XCL_API
File_isDir(File* file);

XCL_EXPORT bool XCL_API
File_isFile(File* file);

XCL_EXPORT bool XCL_API
File_isPathDir(const TChar* path);

XCL_EXPORT bool XCL_API
File_isPathFile(const TChar* path);

XCL_EXPORT bool XCL_API
File_ensure(File* file);

XCL_EXPORT bool XCL_API
File_ensurePath(const TChar* path);

XCL_EXPORT File* XCL_API
File_parentFile(File* file);

XCL_EXPORT int32_t XCL_API
File_parent(File* file, TChar* path, int32_t* len);

XCL_EXPORT int32_t XCL_API
File_path(const File* file, TChar* path, int32_t* len);

XCL_EXPORT FileIter* XCL_API
File_iter(File* file);

XCL_EXPORT FileIter* XCL_API
File_pathIter(const TChar* path);

XCL_EXPORT bool XCL_API
File_listDir(const TChar* path,
             void (*callback)(void*, const TChar*),
             void* obj);

XCL_EXPORT char XCL_API
File_sep();

/**
 * @brief join file path for multiple names
 * @param out output buffer for result path
 * @param outLen length of output buffer
 * @param n args count
 * @param ... multiple args of file name
 * @return true if function successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
File_joinPath(TChar* out, int32_t outLen, unsigned n, ...);

/**
 * @brief join file path for multiple names
 * @param out output buffer for result path
 * @param outLen length of output buffer
 * @param n args count
 * @param names multiple args of file name
 * @return true if function successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
File_joinPath2(TChar* out, int32_t outLen, unsigned n, va_list names);

XCL_EXPORT int32_t XCL_API
FileIter_next(FileIter* fileIter, TChar* fileName, int32_t* len);

XCL_EXPORT void XCL_API
FileIter_delete(FileIter* fileIter);

XCL_EXPORT bool XCL_API
FileIter_current(FileIter* fileIter, TChar* fileName, int32_t* len);

XCL_EXPORT bool XCL_API
FileIter_cwd(FileIter* fileIter, TChar* dir, int32_t* len);

#ifdef __cplusplus
}
#endif