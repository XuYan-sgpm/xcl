#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/util/CRingBuffer.h>

#ifdef _UNICODE
typedef wchar_t __PathChar;
#else
typedef char __PathChar;
#endif

typedef enum
{
    F_READ = 1,
    F_WRITE = 2,
    F_UPDATE = 4,
    F_APPEND = 8
} CFileMode;

typedef struct _CFileCache CFileCache;

typedef struct {
    const uintptr_t fd;
    CFileCache* cache;
    char reserve[16];
} CFile;

typedef struct _CFileIter CFileIter;

XCL_EXPORT CFile XCL_API
File_make(const __PathChar* path, int32_t mode);

XCL_EXPORT bool XCL_API
File_exists(const __PathChar* path);

XCL_EXPORT void XCL_API
File_close(CFile* file);

XCL_EXPORT int32_t XCL_API
File_flush(CFile* file);

XCL_EXPORT int32_t XCL_API
File_read(CFile* file, void* dst, int32_t len);

XCL_EXPORT int32_t XCL_API
File_write(CFile* file, const void* src, int32_t len);

XCL_EXPORT int32_t XCL_API
File_delete(CFile* file);

XCL_EXPORT int32_t XCL_API
File_rename(CFile* file, const __PathChar* newFileName);

XCL_EXPORT bool XCL_API
File_isDir(CFile* file);

XCL_EXPORT bool XCL_API
File_isFile(CFile* file);

XCL_EXPORT int32_t XCL_API
File_ensure(CFile* file);

XCL_EXPORT CFile XCL_API
File_parentFile(CFile* file);

XCL_EXPORT int32_t XCL_API
File_parent(CFile* file, __PathChar* path, int32_t* len);

XCL_EXPORT int32_t XCL_API
File_path(const CFile* file, __PathChar* path, int32_t* len);

XCL_EXPORT CFileIter* XCL_API
File_list(CFile* file);

XCL_EXPORT CFileIter* XCL_API
File_listByPath(const __PathChar* path);

XCL_EXPORT int32_t XCL_API
FileIter_next(CFileIter* fileIter, __PathChar* fileName, int32_t* len);

XCL_EXPORT void XCL_API
FileIter_close(CFileIter* fileIter);

XCL_EXPORT bool XCL_API
FileIter_current(CFileIter* fileIter, __PathChar* fileName, int32_t* len);

XCL_EXPORT bool XCL_API
FileIter_cwd(CFileIter* fileIter, __PathChar* fileName, int32_t* len);

#ifdef __cplusplus
}
#endif