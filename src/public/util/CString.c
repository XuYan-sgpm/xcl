#include "xcl/util/CString.h"
#include "xcl/pool/CPool.h"
#include <assert.h>
#include <string.h>

const static unsigned __STATE_MASK__ = 0x7fffffff;

static inline bool __String_useStack(const CString* string)
{
    return string->state >> 31;
}

static inline int __String_getSize(const CString* string)
{
    return (int)(string->state & __STATE_MASK__);
}

static inline void
__String_setState(CString* string, const int useStack, const int size)
{
    string->state = (useStack << 31u) | (size & __STATE_MASK__);
}

static inline void __String_setSize(CString* string, const int size)
{
    __String_setState(string, __String_useStack(string), size);
}

static inline char* __String_ptr(CString* string, const int pos)
{
    return __String_useStack(string) ? string->mem.stack.data + pos
                                     : string->mem.heap.ptr + pos;
}

static inline const char* __String_cPtr(const CString* string, const int pos)
{
    return __String_useStack(string) ? string->mem.stack.data + pos
                                     : string->mem.heap.ptr + pos;
}

static inline int __String_cap(const CString* string)
{
    return __String_useStack(string) ? (int)sizeof(string->mem.stack)
                                     : string->mem.heap.cap;
}

static inline bool
__String_addCheck(const CString* string, const int pos, const int len)
{
    return pos >= 0 && pos <= __String_getSize(string) && len >= 0 &&
           len <= __String_cap(string) - __String_getSize(string);
}

static inline bool
__String_removeCheck(const CString* string, const int pos, const int len)
{
    return pos >= 0 && pos < __String_getSize(string) && len > 0 &&
           len <= __String_getSize(string) - pos;
}

static inline int __String_grow(const int cap, const int add)
{
    return cap + (cap > add ? cap : add);
}

static void __String_init(CString* string, const int cap)
{
    memset(&string->mem, 0, sizeof(string->mem));
    if (cap <= sizeof(string->mem.stack.data))
    {
        __String_setState(string, true, 0);
    }
    else
    {
        char* p = (char*)Pool_alloc(Pool_def(), cap);
        if (p)
        {
            string->mem.heap.ptr = p;
            string->mem.heap.cap = cap;
        }
        __String_setState(string, false, 0);
    }
}

static char*
__String_beforeInsert(CString* string, const int pos, const int len)
{
    char* insertCursor = NULL;
    char* p = __String_ptr(string, pos);
    const int size = String_size(string);
    const int cap = __String_cap(string);
    if (__String_addCheck(string, pos, len))
    {
        memmove(p + len, p, size - pos);
        __String_setSize(string, size + len);
        insertCursor = p;
    }
    else
    {
        int newCap = __String_grow(cap, size + len - cap);
        char* newPtr = (char*)Pool_alloc(Pool_def(), newCap);
        if (!newPtr)
        {
            return NULL;
        }
        memcpy(newPtr, __String_ptr(string, 0), pos);
        memcpy(newPtr + pos + len, p, size - pos);
        if (!__String_useStack(string))
        {
            Pool_dealloc(Pool_def(),
                         string->mem.heap.ptr,
                         string->mem.heap.cap);
        }
        string->mem.heap.ptr = newPtr;
        string->mem.heap.cap = newCap;
        __String_setState(string, false, size + len);
        insertCursor = newPtr + pos;
    }
    return insertCursor;
}

static char*
__String_beforeAssign(CString* string, const int pos, const int len)
{
    char* assignCursor = NULL;
    const int cap = __String_cap(string);
    char* p = __String_ptr(string, pos);
    if (pos + len <= cap)
    {
        __String_setSize(string, pos + len);
        assignCursor = p;
    }
    else
    {
        int newCap = __String_grow(cap, pos + len - cap);
        char* newPtr = (char*)Pool_alloc(Pool_def(), newCap);
        if (!newPtr)
        {
            return NULL;
        }
        memcpy(newPtr, __String_ptr(string, 0), pos);
        if (!__String_useStack(string))
        {
            Pool_dealloc(Pool_def(),
                         string->mem.heap.ptr,
                         string->mem.heap.cap);
        }
        string->mem.heap.ptr = newPtr;
        string->mem.heap.cap = newCap;
        __String_setState(string, false, pos + len);
        assignCursor = newPtr + pos;
    }
    return assignCursor;
}

static bool
__String_assign(CString* string, const int pos, const char* src, const int len)
{
    char* assignCursor = __String_beforeAssign(string, pos, len);
    if (assignCursor)
    {
        memcpy(assignCursor, src, len);
    }
    return assignCursor;
}

XCL_PUBLIC(CString)
String_make(const char* str)
{
    return String_newRegion(str, strlen(str));
}

XCL_PUBLIC(CString)
String_newRegion(const char* str, const int len)
{
    CString string;
    __String_init(&string, len);
    memcpy(__String_ptr(&string, 0), str, len);
    __String_setSize(&string, len);
    return string;
}

XCL_PUBLIC(CString)
String_alloc(const int cap)
{
    assert(cap > 0);
    CString string;
    __String_init(&string, cap);
    return string;
}

XCL_PUBLIC(void)
String_release(CString* string)
{
    if (!__String_useStack(string))
    {
        Pool_dealloc(Pool_def(), string->mem.heap.ptr, string->mem.heap.cap);
    }
    __String_setState(string, true, 0);
}

XCL_PUBLIC(void)
String_clear(CString* string)
{
    __String_setSize(string, 0);
}

XCL_PUBLIC(int)
String_size(const CString* string)
{
    return __String_getSize(string);
}

XCL_PUBLIC(int)
String_cap(const CString* string)
{
    return __String_cap(string);
}

XCL_PUBLIC(bool)
String_empty(const CString* string)
{
    return __String_getSize(string) == 0;
}

XCL_PUBLIC(bool)
String_assign(CString* string, const char* str)
{
    return __String_assign(string, 0, str, strlen(str));
}

XCL_PUBLIC(bool)
String_assignRegion(CString* string, const char* str, const int len)
{
    return __String_assign(string, 0, str, len);
}

XCL_PUBLIC(bool)
String_assignStr(CString* string, const CString* src)
{
    return __String_assign(string, 0, __String_cPtr(src, 0), String_size(src));
}

XCL_PUBLIC(bool)
String_pushChar(CString* string, char ch)
{
    char* cursor = __String_beforeInsert(string, String_size(string), 1);
    if (cursor)
    {
        *cursor = ch;
    }
    return cursor;
}

XCL_PUBLIC(bool)
String_pushChars(CString* string, const int n, char ch)
{
    char* cursor = __String_beforeInsert(string, String_size(string), n);
    if (cursor)
    {
        memset(cursor, ch, n);
    }
    return cursor;
}

XCL_PUBLIC(bool)
String_pushRegion(CString* string, const char* str, const int len)
{
    char* cursor = __String_beforeInsert(string, String_size(string), len);
    if (cursor)
    {
        memcpy(cursor, str, len);
    }
    return cursor;
}

XCL_PUBLIC(bool)
String_push(CString* string, const char* str)
{
    return String_pushRegion(string, str, strlen(str));
}

XCL_PUBLIC(bool)
String_pushStr(CString* string, const CString* src)
{
    return String_pushRegion(string, __String_cPtr(src, 0), String_size(src));
}

XCL_PUBLIC(bool)
String_writeChar(CString* string, const int pos, char ch)
{
    char* cursor = __String_beforeInsert(string, pos, 1);
    if (cursor)
    {
        *cursor = ch;
    }
    return cursor;
}

XCL_PUBLIC(bool)
String_writeChars(CString* string, const int pos, const int n, char ch)
{
    char* cursor = __String_beforeInsert(string, pos, n);
    if (cursor)
    {
        memset(cursor, ch, n);
    }
    return cursor;
}

XCL_PUBLIC(bool)
String_writeRegion(CString* string,
                   const int pos,
                   const char* str,
                   const int len)
{
    char* cursor = __String_beforeInsert(string, pos, len);
    if (cursor)
    {
        memcpy(cursor, str, len);
    }
    return cursor;
}

XCL_PUBLIC(bool)
String_write(CString* string, const int pos, const char* str)
{
    const int len = strlen(str);
    char* cursor = __String_beforeInsert(string, pos, len);
    if (cursor)
    {
        memcpy(cursor, str, len);
    }
    return cursor;
}

XCL_PUBLIC(bool)
String_writeStr(CString* string, const int pos, const CString* src)
{
    return String_writeRegion(string,
                              pos,
                              __String_cPtr(src, 0),
                              String_size(src));
}

XCL_PUBLIC(bool)
String_pop(CString* string, char* dst)
{
    if (String_empty(string))
    {
        return false;
    }
    char* p = __String_ptr(string, 0);
    *dst = *p;
    const int size = String_size(string);
    memcpy(p, p + 1, size - 1);
    __String_setSize(string, size - 1);
    return true;
}

XCL_PUBLIC(bool)
String_popBack(CString* string, char* dst)
{
    if (String_empty(string))
    {
        return false;
    }
    const int size = String_size(string);
    char* p = __String_ptr(string, size - 1);
    *dst = *p;
    __String_setSize(string, size - 1);
    return true;
}

XCL_PUBLIC(bool)
String_get(const CString* string, const int pos, char* dst)
{
    if (pos >= 0 && pos < String_size(string))
    {
        *dst = *__String_cPtr(string, pos);
        return true;
    }
    return false;
}

XCL_PUBLIC(int)
String_gets(const CString* string, const int pos, const int len, char* dst)
{
    if (!__String_removeCheck(string, pos, len))
    {
        return -1;
    }
    memcpy(dst, __String_cPtr(string, pos), len);
    return len;
}

XCL_PUBLIC(void)
String_delete(CString* string, const int pos)
{
    const int size = String_size(string);
    if (pos < 0 || pos >= size)
    {
        return;
    }
    char* p = __String_ptr(string, pos);
    memcpy(p, p + 1, size - pos - 1);
    __String_setSize(string, size - 1);
}

XCL_PUBLIC(void)
String_deleteRegion(CString* string, const int pos, const int len)
{
    if (__String_removeCheck(string, pos, len))
    {
        char* p = __String_ptr(string, pos);
        const int size = String_size(string);
        memcpy(p, p + len, size - pos - len);
        __String_setSize(string, size - len);
    }
}

XCL_PUBLIC(const char*)
String_queryChar(const CString* string, const bool left, char ch)
{
    const char* p = __String_cPtr(string, 0);
    const int size = String_size(string);
    if (left)
    {
        for (int i = 0; i < size; i++)
        {
            if (*p == ch)
            {
                return p;
            }
        }
    }
    else
    {
        for (int i = size - 1; i >= 0; i--)
        {
            if (*p == ch)
            {
                return p;
            }
        }
    }
    return NULL;
}

// static const char *
// __String_cmp(const char *src, const char *pattern) {
//   const char *cursor1 = src;
//   const char *cursor2 = pattern;
//   while (*cursor1 && *cursor2) {
//     if (*cursor1 != *cursor2) {
//       break;
//     }
//     ++cursor1;
//     ++cursor2;
//   }
//   return cursor1;
// }

// static const char*
//__String_cmpRegion(const char* src, const char* pattern, const int len)
//{
//     int i;
//     for (i = 0; i < len; i++)
//     {
//         if (src[i] != pattern[i])
//         {
//             break;
//         }
//     }
//     return src + i;
// }
//
// static const char*
//__String_directSearch(const CString* string, const char* pattern, const int
// len)
//{
//     for (int i = 0; i < String_size(string) - len; i++)
//     {
//         const char* start = __String_cPtr(string, i);
//         const char* cursor = __String_cmpRegion(start, pattern, len);
//         if (cursor - start == len)
//         {
//             return start;
//         }
//     }
//     return NULL;
// }
//
// const static int STR_QUERY_THRESHOLD = 32;
//
// static void __String_genNext(const char* pattern, int* next, const int len)
//{
//     next[0] = -1;
//     int idx = 0;
//     int val = -1;
//     while (idx < len - 1)
//     {
//         if (val == -1 || pattern[idx] == pattern[val])
//         {
//             if (pattern[++idx] == pattern[++val])
//             {
//                 next[idx] = next[val];
//             }
//             else
//             {
//                 next[idx] = val;
//             }
//         }
//         else
//         {
//             val = next[val];
//         }
//     }
// }
//
// static const char*
//__String_kmpSearch(const CString* string, const char* pattern, const int len)
//{
//     int next[len];
//     __String_genNext(pattern, next, len);
//     int i, j;
//     const char* start = __String_cPtr(string, 0);
//     for (i = 0, j = 0; i < String_size(string) && j < len; i++, j++)
//     {
//         if (j == -1 || start[i] == pattern[j])
//         {
//             ++i;
//             ++j;
//         }
//         else
//         {
//             j = next[j];
//         }
//     }
//     if (j == len)
//     {
//         return &start[i - len];
//     }
//     return NULL;
// }

XCL_PUBLIC(const char*) String_query(const CString* string, const char* str)
{
    return strstr(__String_cPtr(string, 0), str);
}

XCL_PUBLIC(char)
String_at(const CString* string, const int pos)
{
    return *__String_cPtr(string, pos);
}

XCL_PUBLIC(CString)
String_dup(const CString* string)
{
    return String_dupRegion(string, 0, String_size(string));
}

XCL_PUBLIC(CString)
String_dupRegion(const CString* string, const int pos, const int len)
{
    CString copy;
    __String_init(&copy, len);
    memcpy(__String_ptr(&copy, 0), __String_cPtr(string, pos), len);
    __String_setSize(&copy, len);
    return copy;
}
