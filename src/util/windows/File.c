#include <util/File.h>
#include <io.h>
#include <windows.h>
#include <lang/XclErr.h>
#include <stdlib.h>
#include <util/Str.h>

typedef struct {
    unsigned char buf[128];
    int32_t start, finish;
} CFileCache;

static inline void
__FileCache_init(CFileCache* cache)
{
    memset(cache->buf, 0, sizeof(cache->buf));
    cache->start = cache->finish = 0;
}

static int32_t
__FileCache_push(CFileCache* cache, const void* src, int32_t len)
{
    int32_t cap = sizeof(cache->buf);
    int32_t to = cache->finish & (cap - 1);
    int32_t total = 0;
    int32_t size = cache->finish - cache->start;
    if (len > cap - size)
        len = cap - size;
    while (total < len)
    {
        int32_t copy = cap - to;
        if (copy > len - total)
            copy = len - total;
        memcpy(cache->buf + to, (const char*)src + total, copy);
        total += copy;
        to = (to + copy) & (cap - 1);
    }
    cache->finish += total;
    return total;
}

static int32_t
__FileCache_pop(CFileCache* cache, int32_t len)
{
    int32_t size = cache->finish - cache->start;
    if (len > size)
    {
        len = size;
    }
    cache->start += len;
    return len;
}

static inline int32_t
__FileCache_remaining(const CFileCache* cache)
{
    return sizeof(cache->buf) - (cache->finish - cache->start);
}

struct _CFile {
    uintptr_t fd;
    CFileCache cache;
    const __FilePathChr* absPath;
};

static inline const __FilePathChr*
__File_path(const CFile* file)
{
    return file->absPath;
}

static inline void
__File_setPath(CFile* file, const __FilePathChr* path)
{
    __FilePathChr* old = (__FilePathChr*)__File_path(file);
    if (old)
    {
        if (path && _tcscmp(path, old) == 0)
        {
            return;
        }
        free(old);
        file->absPath = NULL;
    }
    if (path)
    {
        file->absPath = _tcsdup(path);
    }
}

static inline bool
__File_isDir(const __FilePathChr* path)
{
    DWORD ret = GetFileAttributes(path);
    if (ret == INVALID_FILE_ATTRIBUTES)
    {
        Err_set(GetLastError());
        return false;
    }
    return ret & FILE_ATTRIBUTE_DIRECTORY;
}

static inline bool
__File_isFile(const __FilePathChr* path)
{
    return !__File_isDir(path);
}

XCL_EXPORT CFile* XCL_API
File_new(const __FilePathChr* path, int32_t mode)
{
    CFile* file = malloc(sizeof(CFile));
    if (!file)
    {
        return NULL;
    }
    file->absPath = NULL;
    DWORD access = 0;
    if ((mode & F_READ) || (mode & F_UPDATE))
    {
        access |= GENERIC_READ;
    }
    if ((mode & F_WRITE) || (mode & F_APPEND))
    {
        access |= GENERIC_WRITE;
    }
    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD openWay = 0;
    if ((mode & F_READ))
    {
        openWay = OPEN_EXISTING;
    }
    else if ((mode & F_WRITE))
    {
        openWay = CREATE_ALWAYS;
    }
    else
    {
        openWay = OPEN_ALWAYS;
    }
    DWORD attrAndFlags = __File_isDir(path)
                             ? FILE_FLAG_BACKUP_SEMANTICS
                             : FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;
    HANDLE h = CreateFile(path, access, shareMode, 0, openWay, attrAndFlags, 0);
    if (h != INVALID_HANDLE_VALUE)
    {
        if ((mode & F_APPEND))
        {
            SetFilePointer(h, 0, NULL, FILE_END);
        }
        __File_setPath(file, path);
        file->fd = (uintptr_t)h;
        __FileCache_init(&file->cache);
    }
    else
    {
        Err_set(GetLastError());
        free(file);
        file = NULL;
    }
    return file;
}

XCL_EXPORT void XCL_API
File_delete(CFile* file)
{
    if (file->fd != (uintptr_t)INVALID_HANDLE_VALUE)
    {
        File_flush(file);
        CloseHandle((HANDLE)file->fd);
        file->fd = (uintptr_t)INVALID_HANDLE_VALUE;
    }
    free(file);
}

XCL_EXPORT int32_t XCL_API
File_flush(CFile* file)
{
    return -1;
}

XCL_EXPORT bool XCL_API
File_isPathExists(const __FilePathChr* path)
{
#ifndef _UNICODE
    return _access(path, 0) == 0;
#else
    return _waccess(path, 0) == 0;
#endif
}

XCL_EXPORT bool XCL_API
File_exists(const CFile* file)
{
    return File_isPathExists(__File_path(file));
}

XCL_EXPORT int32_t XCL_API
File_path(const CFile* file, __FilePathChr* path, int32_t* len)
{
    int32_t filePathLen = _tcslen(__File_path(file));
    if (path && *len < filePathLen)
    {
        return -1;
    }
    if (path)
    {
        memcpy(path, __File_path(file), filePathLen * sizeof(__FilePathChr));
    }
    *len = filePathLen;
    return 0;
}

struct _CFileIter {
    HANDLE findHandle;
    WIN32_FIND_DATA findData;
    const __FilePathChr* cwd;
};

static inline bool
__FileIter_initialized(const CFileIter* fileIter)
{
    return fileIter->findHandle != INVALID_HANDLE_VALUE;
}

// static inline void
// __FileIter_setInit(CFileIter* fileIter, bool flag)
// {
//     char state = fileIter->state;
//     fileIter->state = flag ? (state | 1) : (state & ~1);
// }

// static inline bool
// __FileIter_isUsingFile(const CFileIter* fileIter)
// {
//     return fileIter->state & 2;
// }

// static inline void
// __FileIter_useFile(CFileIter* fileIter, bool isFile)
// {
//     char state = fileIter->state;
//     fileIter->state = isFile ? (state | 2) : (state & ~2);
// }

XCL_EXPORT CFileIter* XCL_API
File_list(CFile* file)
{
    if (!__File_isDir(__File_path(file)))
    {
        return 0;
    }
    CFileIter* iter = malloc(sizeof(CFileIter));
    if (!iter)
    {
        return 0;
    }
    iter->findHandle = INVALID_HANDLE_VALUE;
    iter->cwd = file->absPath;
    return iter;
}

XCL_EXPORT CFileIter* XCL_API
File_listByPath(const __FilePathChr* path)
{
    if (!__File_isDir(path))
    {
        return NULL;
    }
    CFileIter* iter = malloc(sizeof(CFileIter));
    if (!iter)
    {
        return 0;
    }
    iter->findHandle = INVALID_HANDLE_VALUE;
    iter->cwd = _tcsdup(path);
    return iter;
}

XCL_EXPORT int32_t XCL_API
FileIter_next(CFileIter* fileIter, __FilePathChr* fileName, int32_t* len)
{
    for (;;)
    {
        if (!__FileIter_initialized(fileIter))
        {
            __FilePathChr findDir[1024];
            int32_t findLen = sizeof(findDir) / sizeof(__FilePathChr);
            const __FilePathChr* cwd;
            size_t cwdLen;
            cwd = fileIter->cwd;
            cwdLen = _tcslen(cwd);
            if (cwdLen >= findLen)
            {
                return -1;
            }
            _tcscpy(findDir, cwd);
            findLen = cwdLen;
            _tcscpy(findDir + findLen,
                    findDir[findLen - 1] == '\\' ? __TCSTR("*.*")
                                                 : __TCSTR("\\*.*"));
            HANDLE h = FindFirstFile(findDir, &fileIter->findData);
            if (h == INVALID_HANDLE_VALUE)
            {
                Err_set(GetLastError());
                return -1;
            }
            fileIter->findHandle = h;
        }
        else
        {
            if (!FindNextFile(fileIter->findHandle, &fileIter->findData))
            {
                DWORD err = GetLastError();
                if (err == ERROR_NO_MORE_FILES)
                {
                    return 0;
                }
                else
                {
                    Err_set(err);
                    return -1;
                }
            }
        }
        if (_tcscmp(fileIter->findData.cFileName, __TCSTR(".")) != 0
            && _tcscmp(fileIter->findData.cFileName, __TCSTR("..")) != 0)
        {
            __FilePathChr* p = fileIter->findData.cFileName;
            size_t nameLen = _tcslen(p);
            if (fileName && *len < nameLen)
            {
                return -1;
            }
            _tcsncpy(fileName, p, nameLen);
            *len = nameLen;
            break;
        }
    }
    return 1;
}

XCL_EXPORT bool XCL_API
FileIter_current(CFileIter* fileIter, __FilePathChr* fileName, int32_t* len)
{
    if (!__FileIter_initialized(fileIter))
    {
        return false;
    }
    int32_t req = _tcslen(fileIter->findData.cFileName);
    if (fileName && *len < req)
    {
        return false;
    }
    if (fileName)
    {
        _tcsncpy(fileName, fileIter->findData.cFileName, req);
    }
    *len = req;
    return true;
}

XCL_EXPORT bool XCL_API
FileIter_cwd(CFileIter* fileIter, __FilePathChr* dir, int32_t* len)
{
    if (!__FileIter_initialized(fileIter))
    {
        return false;
    }
    int32_t cwdLen = _tcslen(fileIter->cwd);
    if (dir && *len < cwdLen)
    {
        return false;
    }
    if (dir)
        _tcscpy(dir, fileIter->cwd);
    *len = cwdLen;
    return true;
}

XCL_EXPORT bool XCL_API
File_isDir(CFile* file)
{
    return __File_isDir(__File_path(file));
}

XCL_EXPORT bool XCL_API
File_isFile(CFile* file)
{
    return __File_isFile(__File_path(file));
}

XCL_EXPORT bool XCL_API
File_isPathDir(const __FilePathChr* path)
{
    return __File_isDir(path);
}

XCL_EXPORT bool XCL_API
File_isPathFile(const __FilePathChr* path)
{
    return __File_isFile(path);
}
