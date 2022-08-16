#include <xcl/util/CFile.h>
#include <io.h>
#include <windows.h>
#include <xcl/lang/XclErr.h>
#include <stdlib.h>
#include <xcl/util/CStrDef.h>

#ifdef _UNICODE
#define __TCSTR(str) L##str
#else
#define __TCSTR(str) str
#endif

static inline const __PathChar*
__File_path(const CFile* file)
{
    return (const __PathChar*)(*(uintptr_t*)file->reserve);
}

static inline void
__File_setPath(CFile* file, const __PathChar* newPath)
{
    const __PathChar* oldPath = __File_path(file);
    if (oldPath)
    {
        if (_tcscmp(oldPath, newPath) == 0)
        {
            return;
        }
        free((void*)oldPath);
        *(uintptr_t*)file->reserve = 0;
    }
    if (newPath)
    {
        size_t len = _tcslen(newPath);
        __PathChar* dup = malloc((len + 1) * sizeof(__PathChar));
        if (!dup)
        {
            return;
        }
        _tcscpy(dup, newPath);
        *(uintptr_t*)file->reserve = (uintptr_t)dup;
    }
}

XCL_EXPORT CFile XCL_API
File_make(const __PathChar* path, int32_t mode)
{
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
    HANDLE h = CreateFile(path,
                          access,
                          shareMode,
                          0,
                          openWay,
                          FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,
                          0);
    CFile file = {.fd = (uintptr_t)h, .cache = 0};
    if (h != INVALID_HANDLE_VALUE)
    {
        if ((mode & F_APPEND))
        {
            SetFilePointer(h, 0, NULL, FILE_END);
        }
        __File_setPath(&file, path);
    }
    else
    {
        __File_setPath(&file, NULL);
    }
    return file;
}

XCL_EXPORT void XCL_API
File_close(CFile* file)
{
    __PathChar* path;
    if ((path = (__PathChar*)__File_path(file)))
    {
        File_flush(file);
        CloseHandle((HANDLE)file->fd);
        __File_setPath(file, NULL);
        free(path);
    }
}

XCL_EXPORT int32_t XCL_API
File_flush(CFile* file)
{}

XCL_EXPORT bool XCL_API
File_exists(const __PathChar* path)
{
    return _access(path, 0) == 0;
}

XCL_EXPORT int32_t XCL_API
File_path(const CFile* file, __PathChar* path, int32_t* len)
{
    HANDLE handle = (HANDLE)file->fd;
    char buf[sizeof(FILE_NAME_INFO) + 2047];
    if (!GetFileInformationByHandleEx(handle, FileNameInfo, buf, sizeof(buf)))
    {
        Err_set(GetLastError());
        return -1;
    }
    FILE_NAME_INFO* info = (FILE_NAME_INFO*)buf;
    if (!path)
    {
        *len = info->FileNameLength;
    }
    else if (*len < info->FileNameLength)
    {
        Err_set(XCL_INSUFFICIENT_MEMORY);
        return -1;
    }
    else
    {
        memcpy(path,
               (void*)info->FileName,
               info->FileNameLength * sizeof(info->FileName[0]));
        *len = info->FileNameLength;
    }
    return 0;
}

struct _CFileIter {
    HANDLE findHandle;
    WIN32_FIND_DATA findData;
    char state;
    union
    {
        CFile* file;
        const __PathChar* path;
    } __associate;
};

static inline bool
__FileIter_initialized(const CFileIter* fileIter)
{
    return fileIter->state & 1;
}

static inline void
__FileIter_setInit(CFileIter* fileIter, bool flag)
{
    char state = fileIter->state;
    fileIter->state = flag ? (state | 1) : (state & ~1);
}

static inline bool
__FileIter_isUsingFile(const CFileIter* fileIter)
{
    return fileIter->state & 2;
}

static inline void
__FileIter_useFile(CFileIter* fileIter, bool isFile)
{
    char state = fileIter->state;
    fileIter->state = isFile ? (state | 2) : (state & ~2);
}

XCL_EXPORT CFileIter* XCL_API
File_list(CFile* file)
{
    CFileIter* iter = malloc(sizeof(CFileIter));
    if (!iter)
    {
        return 0;
    }
    __FileIter_setInit(iter, false);
    __FileIter_useFile(iter, true);
    iter->__associate.file = file;
    return iter;
}

XCL_EXPORT CFileIter* XCL_API
File_listByPath(const __PathChar* path)
{
    CFileIter* iter = malloc(sizeof(CFileIter));
    if (!iter)
    {
        return 0;
    }
    __FileIter_setInit(iter, false);
    __FileIter_useFile(iter, false);
    size_t len = _tcslen(path);
    __PathChar* dup = malloc((len + 1) * sizeof(__PathChar));
    if (!dup)
    {
        free(iter);
        return NULL;
    }
    _tcscpy(dup, path);
    iter->__associate.path = dup;
    return iter;
}

XCL_EXPORT int32_t XCL_API
FileIter_next(CFileIter* fileIter, __PathChar* fileName, int32_t* len)
{
    for (;;)
    {
        if (!__FileIter_initialized(fileIter))
        {
            __PathChar dir[1024];
            int32_t dirLen = sizeof(dir) / sizeof(__PathChar);
            if (__FileIter_isUsingFile(fileIter))
            {
                if (File_path(fileIter->__associate.file, dir, &dirLen) != 0)
                {
                    return -1;
                }
                dir[dirLen] = 0;
            }
            else
            {
                dirLen = _tcslen(fileIter->__associate.path);
                _tcscpy(dir, fileIter->__associate.path);
            }
            _tcscpy(dir + dirLen,
                    dir[dirLen - 1] == '\\' ? __TCSTR("*.*")
                                            : __TCSTR("\\*.*"));
            HANDLE h = FindFirstFile(dir, &fileIter->findData);
            if (h == INVALID_HANDLE_VALUE)
            {
                Err_set(GetLastError());
                return -1;
            }
            fileIter->findHandle = h;
            __FileIter_setInit(fileIter, true);
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
            __PathChar* p = fileIter->findData.cFileName;
            size_t nameLen = _tcslen(p);
            if (fileName && *len < nameLen)
            {
                Err_set(XCL_INSUFFICIENT_MEMORY);
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
FileIter_current(CFileIter* fileIter, __PathChar* fileName, int32_t* len)
{
    if (!__FileIter_initialized(fileIter))
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
    int32_t req = _tcslen(fileIter->findData.cFileName);
    if (fileName && *len < req)
    {
        Err_set(XCL_INSUFFICIENT_MEMORY);
        return false;
    }
    if (!fileName)
    {
        *len = req;
    }
    else
    {
        _tcsncpy(fileName, fileIter->findData.cFileName, req);
        *len = req;
    }
    return true;
}

XCL_EXPORT bool XCL_API
FileIter_cwd(CFileIter* fileIter, __PathChar* fileName, int32_t* len)
{
    if (!__FileIter_initialized(fileIter))
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
    if (__FileIter_isUsingFile(fileIter))
    {
        if (File_path(fileIter->__associate.file, fileName, len) != 0)
        {
            return false;
        }
    }
    else
    {
        int32_t dirLen = _tcslen(fileIter->__associate.path);
        if (fileName && *len < dirLen)
        {
            Err_set(XCL_INSUFFICIENT_MEMORY);
            return false;
        }
        if (fileName)
            _tcscpy(fileName, fileIter->__associate.path);
        *len = dirLen;
    }
    return true;
}
