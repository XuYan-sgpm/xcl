#include <io/file.h>
#include <io.h>
#include <windows.h>
#include <xcl/lang/xcl_err.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

typedef struct FileCache {
    unsigned char buf[128];
    int32_t start, finish;
} FileCache;

static inline void
__FileCache_init(FileCache* cache)
{
    memset(cache->buf, 0, sizeof(cache->buf));
    cache->start = cache->finish = 0;
}

static int32_t
__FileCache_push(FileCache* cache, const void* src, int32_t len)
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
__FileCache_pop(FileCache* cache, int32_t len)
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
__FileCache_remaining(const FileCache* cache)
{
    return sizeof(cache->buf) - (cache->finish - cache->start);
}

struct File {
    uintptr_t fd;
    FileCache cache;
    const TChar* abs_path;
};

static inline const TChar*
__File_path(const File* file)
{
    return file->abs_path;
}

static inline void
__File_setPath(File* file, const TChar* path)
{
    TChar* old = (TChar*)__File_path(file);
    if (old)
    {
        if (path && _tcscmp(path, old) == 0)
        {
            return;
        }
        free(old);
        file->abs_path = NULL;
    }
    if (path)
    {
        file->abs_path = _tcsdup(path);
    }
}

static inline bool
__File_isDir(const TChar* path)
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
__File_isFile(const TChar* path)
{
    return !__File_isDir(path);
}

XCL_EXPORT File* XCL_API
File_new(const TChar* path, int32_t mode)
{
    File* file = malloc(sizeof(File));
    if (!file)
    {
        return NULL;
    }
    file->abs_path = NULL;
    DWORD access = 0;
    if ((mode & F_READ) || (mode & F_UPDATE))
    {
        access |= GENERIC_READ;
    }
    if ((mode & F_WRITE) || (mode & F_APPEND))
    {
        access |= GENERIC_WRITE;
    }
    DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD open_way = 0;
    if ((mode & F_READ))
    {
        open_way = OPEN_EXISTING;
    }
    else if ((mode & F_WRITE))
    {
        open_way = CREATE_ALWAYS;
    }
    else
    {
        open_way = OPEN_ALWAYS;
    }
    DWORD attr_and_flags
        = __File_isDir(path) ? FILE_FLAG_BACKUP_SEMANTICS
                             : FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;
    HANDLE h
        = CreateFile(path, access, share_mode, 0, open_way, attr_and_flags, 0);
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
File_delete(File* file)
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
File_flush(File* file)
{
    return -1;
}

XCL_EXPORT bool XCL_API
File_isPathExists(const TChar* path)
{
#ifndef _UNICODE
    return _access(path, 0) == 0;
#else
    return _waccess(path, 0) == 0;
#endif
}

XCL_EXPORT bool XCL_API
File_exists(const File* file)
{
    return File_isPathExists(__File_path(file));
}

XCL_EXPORT int32_t XCL_API
File_path(const File* file, TChar* path, int32_t* len)
{
    int32_t file_path_len = _tcslen(__File_path(file));
    if (path)
    {
        if (*len > file_path_len)
            *len = file_path_len;
        _tcsncpy(path, __File_path(file), *len);
    }
    else
    {
        *len = file_path_len;
    }
    return 0;
}

struct FileIter {
    HANDLE find_handle;
    WIN32_FIND_DATA find_data;
    const TChar* cwd;
};

static inline bool
__FileIter_initialized(const FileIter* file_iter)
{
    return file_iter->find_handle != INVALID_HANDLE_VALUE;
}

XCL_EXPORT FileIter* XCL_API
File_iter(File* file)
{
    if (!__File_isDir(__File_path(file)))
    {
        return 0;
    }
    FileIter* iter = malloc(sizeof(FileIter));
    if (!iter)
    {
        return 0;
    }
    iter->find_handle = INVALID_HANDLE_VALUE;
    iter->cwd = _tcsdup(file->abs_path);
    return iter;
}

XCL_EXPORT FileIter* XCL_API
File_pathIter(const TChar* path)
{
    if (!__File_isDir(path))
    {
        return NULL;
    }
    FileIter* iter = malloc(sizeof(FileIter));
    if (!iter)
    {
        return 0;
    }
    iter->find_handle = INVALID_HANDLE_VALUE;
    iter->cwd = _tcsdup(path);
    return iter;
}

XCL_EXPORT bool XCL_API
File_listDir(const TChar* path,
             void (*callback)(void*, const TChar*),
             void* obj)
{
    if (!File_isPathExists(path))
    {
        return false;
    }
    if (File_isPathFile(path))
    {
        return false;
    }
    int32_t path_len = _tcslen(path);
    int32_t postfix = 3;
    if (path[path_len - 1] != '\\')
    {
        ++postfix;
    }
    TChar tmp[2048];
    if (!tmp)
    {
        return false;
    }
    bool success = false;
    _tcscpy(tmp, path);
    _tcscat(tmp, postfix > 3 ? __TCSTR("\\*.*") : __TCSTR("*.*"));
    WIN32_FIND_DATA find_data;
    memset(&find_data, 0, sizeof(find_data));
    HANDLE h = FindFirstFile(tmp, &find_data);
    if (h == INVALID_HANDLE_VALUE)
    {
        Err_set(GetLastError());
        goto end;
    }
    do
    {
        if (strcmp(find_data.cFileName, "..") == 0
            || strcmp(find_data.cFileName, ".") == 0)
        {
        }
        else if (callback)
        {
            callback(obj, find_data.cFileName);
        }
    } while (FindNextFile(h, &find_data));
    int err = GetLastError();
    success = true;
end:
    FindClose(h);
    return success;
}

XCL_EXPORT int32_t XCL_API
FileIter_next(FileIter* file_iter, TChar* file_name, int32_t* len)
{
    for (;;)
    {
        if (!__FileIter_initialized(file_iter))
        {
            const TChar* cwd;
            size_t cwd_len;
            cwd = file_iter->cwd;
            cwd_len = _tcslen(cwd);
            TChar find_dir[2048];
            int32_t find_len = sizeof(find_dir) / sizeof(TChar);
            assert(find_len > cwd_len);
            _tcscpy(find_dir, cwd);
            find_len = cwd_len;
            _tcscpy(find_dir + find_len,
                    find_dir[find_len - 1] == '\\' ? __TCSTR("*.*")
                                                   : __TCSTR("\\*.*"));
            HANDLE h = FindFirstFile(find_dir, &file_iter->find_data);
            if (h == INVALID_HANDLE_VALUE)
            {
                Err_set(GetLastError());
                return -1;
            }
            file_iter->find_handle = h;
        }
        else
        {
            if (!FindNextFile(file_iter->find_handle, &file_iter->find_data))
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
        if (_tcscmp(file_iter->find_data.cFileName, __TCSTR(".")) != 0
            && _tcscmp(file_iter->find_data.cFileName, __TCSTR("..")) != 0)
        {
            TChar* p = file_iter->find_data.cFileName;
            size_t name_len = _tcslen(p);
            if (file_name)
            {
                if (*len > name_len)
                    *len = name_len;
                _tcsncpy(file_name, p, *len);
            }
            else
            {
                *len = name_len;
            }
            break;
        }
    }
    return 1;
}

XCL_EXPORT bool XCL_API
FileIter_current(FileIter* file_iter, TChar* file_name, int32_t* len)
{
    if (!__FileIter_initialized(file_iter))
    {
        return false;
    }
    int32_t req = _tcslen(file_iter->find_data.cFileName);
    if (file_name)
    {
        if (*len > req)
            *len = req;
        _tcsncpy(file_name, file_iter->find_data.cFileName, *len);
    }
    else
    {
        *len = req;
    }
    return true;
}

XCL_EXPORT bool XCL_API
FileIter_cwd(FileIter* file_iter, TChar* dir, int32_t* len)
{
    if (!__FileIter_initialized(file_iter))
    {
        return false;
    }
    int32_t cwd_len = _tcslen(file_iter->cwd);
    if (dir)
    {
        if (*len > cwd_len)
            *len = cwd_len;
        _tcsncpy(dir, file_iter->cwd, *len);
    }
    else
    {
        *len = cwd_len;
    }
    return true;
}

XCL_EXPORT void XCL_API
FileIter_delete(FileIter* file_iter)
{
    if (file_iter->find_handle != INVALID_HANDLE_VALUE)
    {
        FindClose(file_iter->find_handle);
        file_iter->find_handle = INVALID_HANDLE_VALUE;
    }
    if (file_iter->cwd)
    {
        free((void*)file_iter->cwd);
        file_iter->cwd = NULL;
    }
    free(file_iter);
}

XCL_EXPORT bool XCL_API
File_isDir(File* file)
{
    return __File_isDir(__File_path(file));
}

XCL_EXPORT bool XCL_API
File_isFile(File* file)
{
    return __File_isFile(__File_path(file));
}

XCL_EXPORT bool XCL_API
File_isPathDir(const TChar* path)
{
    return __File_isDir(path);
}

XCL_EXPORT bool XCL_API
File_isPathFile(const TChar* path)
{
    return __File_isFile(path);
}

XCL_EXPORT char XCL_API
File_sep()
{
    return '\\';
}

XCL_EXPORT bool XCL_API
File_joinPath(TChar* out, int32_t out_len, unsigned n, ...)
{
    if (!out)
    {
        return false;
    }
    if (n == 0)
    {
        out[0] = 0;
    }
    else
    {
        va_list names;
        va_start(names, n);
        bool ret = File_joinPath2(out, out_len, n, names);
        va_end(names);
        return ret;
    }
    return true;
}

XCL_EXPORT bool XCL_API
File_joinPath2(TChar* out, int32_t out_len, unsigned n, va_list names)
{
    if (!out)
    {
        return false;
    }
    if (n == 0)
    {
        out[0] = 0;
    }
    else
    {
        int32_t cursor = 0;
        for (int i = 0; i < n; i++)
        {
            if (i > 0)
            {
                if (out[cursor - 1] != '\\')
                {
                    out[cursor++] = '\\';
                }
            }
            const TChar* name = va_arg(names, const TChar*);
            int32_t name_len = _tcslen(name);
            if (name_len > out_len - cursor - 1)
            {
                break;
            }
            else if (out + cursor != name)
            {
                _tcscpy(out + cursor, name);
            }
            cursor += name_len;
        }
        out[cursor] = 0;
    }
    return true;
}

XCL_EXPORT bool XCL_API
File_deletePath(const TChar* path)
{
    if (!File_isPathExists(path))
    {
        return false;
    }
    if (File_isPathFile(path))
    {
        if (!DeleteFile(path))
        {
            Err_set(GetLastError());
            return false;
        }
        return true;
    }
    int32_t path_len = _tcslen(path);
    int32_t postfix = 3;
    if (path[path_len - 1] != '\\')
    {
        ++postfix;
    }
    TChar tmp[2048];
    if (!tmp)
    {
        return false;
    }
    bool success = false;
    _tcscpy(tmp, path);
    _tcscat(tmp, postfix > 3 ? __TCSTR("\\*.*") : __TCSTR("*.*"));
    WIN32_FIND_DATA find_data;
    HANDLE h = FindFirstFile(tmp, &find_data);
    if (h == INVALID_HANDLE_VALUE)
    {
        Err_set(GetLastError());
        goto end;
    }
    do
    {
        if (strcmp(find_data.cFileName, "..") == 0
            || strcmp(find_data.cFileName, ".") == 0)
        {
        }
        else
        {
            if (!File_deletePath(find_data.cFileName))
            {
                goto end;
            }
        }
    } while (FindNextFile(h, &find_data));
    success = true;
end:
    if (h != INVALID_HANDLE_VALUE)
        FindClose(h);
    return success;
}
