#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/util/RingBuffer.h>
#include <xcl/util/Str.h>

typedef enum
{
    F_READ = 1,
    F_WRITE = 2,
    F_UPDATE = 4,
    F_APPEND = 8
} CFileMode;

typedef struct _CFile CFile;
typedef struct _CFileIter CFileIter;

XCL_EXPORT CFile* XCL_API
File_new(const __FilePathChr* path, int32_t mode);

XCL_EXPORT bool XCL_API
File_isPathExists(const __FilePathChr* path);

XCL_EXPORT bool XCL_API
File_exists(const CFile* file);

XCL_EXPORT void XCL_API
File_delete(CFile* file);

XCL_EXPORT int32_t XCL_API
File_flush(CFile* file);

XCL_EXPORT int32_t XCL_API
File_read(CFile* file, void* dst, int32_t len);

XCL_EXPORT int32_t XCL_API
File_write(CFile* file, const void* src, int32_t len);

XCL_EXPORT bool XCL_API
File_remove(CFile* file);

XCL_EXPORT bool XCL_API
File_deletePath(const __FilePathChr* path);

XCL_EXPORT bool XCL_API
File_rename(CFile* file, const __FilePathChr* newFileName);

XCL_EXPORT bool XCL_API
File_renamePath(const __FilePathChr* old, const __FilePathChr* now);

XCL_EXPORT bool XCL_API
File_isDir(CFile* file);

XCL_EXPORT bool XCL_API
File_isFile(CFile* file);

XCL_EXPORT bool XCL_API
File_isPathDir(const __FilePathChr* path);

XCL_EXPORT bool XCL_API
File_isPathFile(const __FilePathChr* path);

XCL_EXPORT bool XCL_API
File_ensure(CFile* file);

XCL_EXPORT bool XCL_API
File_ensurePath(const __FilePathChr* path);

XCL_EXPORT CFile* XCL_API
File_parentFile(CFile* file);

XCL_EXPORT int32_t XCL_API
File_parent(CFile* file, __FilePathChr* path, int32_t* len);

XCL_EXPORT int32_t XCL_API
File_path(const CFile* file, __FilePathChr* path, int32_t* len);

XCL_EXPORT CFileIter* XCL_API
File_list(CFile* file);

XCL_EXPORT CFileIter* XCL_API
File_listByPath(const __FilePathChr* path);

/**
 * @brief join file path for multiple names
 * @param out output buffer for result path
 * @param outLen length of output buffer
 * @param n args count
 * @param ... multiple args of file name
 * @return true if function successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
File_joinPath(__FilePathChr* out, int32_t outLen, unsigned n, ...);

/**
 * @brief join file path for multiple names
 * @param out output buffer for result path
 * @param outLen length of output buffer
 * @param n args count
 * @param names multiple args of file name
 * @return true if function successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
File_joinPath2(__FilePathChr* out, int32_t outLen, unsigned n, va_list names);

XCL_EXPORT int32_t XCL_API
FileIter_next(CFileIter* fileIter, __FilePathChr* fileName, int32_t* len);

XCL_EXPORT void XCL_API
FileIter_delete(CFileIter* fileIter);

XCL_EXPORT bool XCL_API
FileIter_current(CFileIter* fileIter, __FilePathChr* fileName, int32_t* len);

XCL_EXPORT bool XCL_API
FileIter_cwd(CFileIter* fileIter, __FilePathChr* dir, int32_t* len);

#ifdef __cplusplus
}
#endif