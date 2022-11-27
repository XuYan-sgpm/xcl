#include "util/xcl_string.h"
#include "pool/pool.h"
#include <assert.h>
#include <string.h>

const static unsigned __STATE_MASK__ = 0x7fffffff;

static inline bool
__String_useStack(const String* string)
{
    return string->state >> 31;
}

static inline int
__String_getSize(const String* string)
{
    return (int)(string->state & __STATE_MASK__);
}

static inline void
__String_setState(String* string, const int use_stack, const int size)
{
    string->state = (use_stack << 31u) | (size & __STATE_MASK__);
}

static inline void
__String_setSize(String* string, const int size)
{
    __String_setState(string, __String_useStack(string), size);
}

static inline char*
__String_ptr(String* string, const int pos)
{
    return __String_useStack(string) ? string->mem.stack.data + pos
                                     : string->mem.heap.ptr + pos;
}

static inline const char*
__String_cPtr(const String* string, const int pos)
{
    return __String_useStack(string) ? string->mem.stack.data + pos
                                     : string->mem.heap.ptr + pos;
}

static inline int
__String_cap(const String* string)
{
    return __String_useStack(string) ? (int)sizeof(string->mem.stack)
                                     : string->mem.heap.cap;
}

static inline bool
__String_addCheck(const String* string, const int pos, const int len)
{
    return pos >= 0 && pos <= __String_getSize(string) && len >= 0 &&
           len <= __String_cap(string) - __String_getSize(string);
}

static inline bool
__String_removeCheck(const String* string, const int pos, const int len)
{
    return pos >= 0 && pos < __String_getSize(string) && len > 0 &&
           len <= __String_getSize(string) - pos;
}

static inline int
__String_grow(const int cap, const int add)
{
    return cap + (cap > add ? cap : add);
}

static void
__String_init(String* string, const int cap)
{
    memset(&string->mem, 0, sizeof(string->mem));
    if (cap <= sizeof(string->mem.stack.data))
    {
        __String_setState(string, true, 0);
    }
    else
    {
        char* p = (char*)Pool_alloc(Pool_global(), cap);
        if (p)
        {
            string->mem.heap.ptr = p;
            string->mem.heap.cap = cap;
        }
        __String_setState(string, false, 0);
    }
}

static char*
__String_insertAux(String* string, const int pos, const int len)
{
    char* insert_cursor = NULL;
    char* p = __String_ptr(string, pos);
    const int size = String_size(string);
    const int cap = __String_cap(string);
    if (__String_addCheck(string, pos, len))
    {
        memmove(p + len, p, size - pos);
        __String_setSize(string, size + len);
        insert_cursor = p;
    }
    else
    {
        int new_cap = __String_grow(cap, size + len - cap);
        char* new_ptr = (char*)Pool_alloc(Pool_global(), new_cap);
        if (!new_ptr)
        {
            return NULL;
        }
        memcpy(new_ptr, __String_ptr(string, 0), pos);
        memcpy(new_ptr + pos + len, p, size - pos);
        if (!__String_useStack(string))
        {
            Pool_dealloc(Pool_global(),
                         string->mem.heap.ptr,
                         string->mem.heap.cap);
        }
        string->mem.heap.ptr = new_ptr;
        string->mem.heap.cap = new_cap;
        __String_setState(string, false, size + len);
        insert_cursor = new_ptr + pos;
    }
    return insert_cursor;
}

static char*
__String_assignAux(String* string, const int pos, const int len)
{
    char* assign_cursor = NULL;
    const int cap = __String_cap(string);
    char* p = __String_ptr(string, pos);
    if (pos + len <= cap)
    {
        __String_setSize(string, pos + len);
        assign_cursor = p;
    }
    else
    {
        int new_cap = __String_grow(cap, pos + len - cap);
        char* new_ptr = (char*)Pool_alloc(Pool_global(), new_cap);
        if (!new_ptr)
        {
            return NULL;
        }
        memcpy(new_ptr, __String_ptr(string, 0), pos);
        if (!__String_useStack(string))
        {
            Pool_dealloc(Pool_global(),
                         string->mem.heap.ptr,
                         string->mem.heap.cap);
        }
        string->mem.heap.ptr = new_ptr;
        string->mem.heap.cap = new_cap;
        __String_setState(string, false, pos + len);
        assign_cursor = new_ptr + pos;
    }
    return assign_cursor;
}

static bool
__String_assign(String* string, const int pos, const char* src, const int len)
{
    char* assign_cursor = __String_assignAux(string, pos, len);
    if (assign_cursor)
    {
        memcpy(assign_cursor, src, len);
    }
    return assign_cursor;
}

String XCL_API
String_make(const char* str)
{
    return String_newRegion(str, strlen(str));
}

String XCL_API
String_newRegion(const char* str, const int len)
{
    String string;
    __String_init(&string, len);
    memcpy(__String_ptr(&string, 0), str, len);
    __String_setSize(&string, len);
    return string;
}

String XCL_API
String_alloc(const int cap)
{
    assert(cap > 0);
    String string;
    __String_init(&string, cap);
    return string;
}

void XCL_API
String_release(String* string)
{
    if (!__String_useStack(string))
    {
        Pool_dealloc(Pool_global(), string->mem.heap.ptr, string->mem.heap.cap);
    }
    __String_setState(string, true, 0);
}

void XCL_API
String_clear(String* string)
{
    __String_setSize(string, 0);
}

int XCL_API
String_size(const String* string)
{
    return __String_getSize(string);
}

int XCL_API
String_cap(const String* string)
{
    return __String_cap(string);
}

bool XCL_API
String_empty(const String* string)
{
    return __String_getSize(string) == 0;
}

bool XCL_API
String_assign(String* string, const char* str)
{
    return __String_assign(string, 0, str, strlen(str));
}

bool XCL_API
String_assignRegion(String* string, const char* str, const int len)
{
    return __String_assign(string, 0, str, len);
}

bool XCL_API
String_assignStr(String* string, const String* src)
{
    return __String_assign(string, 0, __String_cPtr(src, 0), String_size(src));
}

bool XCL_API
String_pushChar(String* string, char ch)
{
    char* cursor = __String_insertAux(string, String_size(string), 1);
    if (cursor)
    {
        *cursor = ch;
    }
    return cursor;
}

bool XCL_API
String_pushChars(String* string, const int n, char ch)
{
    char* cursor = __String_insertAux(string, String_size(string), n);
    if (cursor)
    {
        memset(cursor, ch, n);
    }
    return cursor;
}

bool XCL_API
String_pushRegion(String* string, const char* str, const int len)
{
    char* cursor = __String_insertAux(string, String_size(string), len);
    if (cursor)
    {
        memcpy(cursor, str, len);
    }
    return cursor;
}

bool XCL_API
String_push(String* string, const char* str)
{
    return String_pushRegion(string, str, strlen(str));
}

bool XCL_API
String_pushStr(String* string, const String* src)
{
    return String_pushRegion(string, __String_cPtr(src, 0), String_size(src));
}

bool XCL_API
String_writeChar(String* string, const int pos, char ch)
{
    char* cursor = __String_insertAux(string, pos, 1);
    if (cursor)
    {
        *cursor = ch;
    }
    return cursor;
}

bool XCL_API
String_writeChars(String* string, const int pos, const int n, char ch)
{
    char* cursor = __String_insertAux(string, pos, n);
    if (cursor)
    {
        memset(cursor, ch, n);
    }
    return cursor;
}

bool XCL_API
String_writeRegion(String* string,
                   const int pos,
                   const char* str,
                   const int len)
{
    char* cursor = __String_insertAux(string, pos, len);
    if (cursor)
    {
        memcpy(cursor, str, len);
    }
    return cursor;
}

bool XCL_API
String_write(String* string, const int pos, const char* str)
{
    const int len = strlen(str);
    char* cursor = __String_insertAux(string, pos, len);
    if (cursor)
    {
        memcpy(cursor, str, len);
    }
    return cursor;
}

bool XCL_API
String_writeStr(String* string, const int pos, const String* src)
{
    return String_writeRegion(string,
                              pos,
                              __String_cPtr(src, 0),
                              String_size(src));
}

bool XCL_API
String_pop(String* string, char* dst)
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

bool XCL_API
String_popBack(String* string, char* dst)
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

bool XCL_API
String_get(const String* string, const int pos, char* dst)
{
    if (pos >= 0 && pos < String_size(string))
    {
        *dst = *__String_cPtr(string, pos);
        return true;
    }
    return false;
}

int XCL_API
String_gets(const String* string, const int pos, const int len, char* dst)
{
    if (!__String_removeCheck(string, pos, len))
    {
        return -1;
    }
    memcpy(dst, __String_cPtr(string, pos), len);
    return len;
}

void XCL_API
String_delete(String* string, const int pos)
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

void XCL_API
String_deleteRegion(String* string, const int pos, const int len)
{
    if (__String_removeCheck(string, pos, len))
    {
        char* p = __String_ptr(string, pos);
        const int size = String_size(string);
        memcpy(p, p + len, size - pos - len);
        __String_setSize(string, size - len);
    }
}

const char* XCL_API
String_queryChar(const String* string, const bool left, char ch)
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
//__String_directSearch(const String* string, const char* pattern, const int
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
//__String_kmpSearch(const String* string, const char* pattern, const int len)
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

const char* XCL_API
String_query(const String* string, const char* str)
{
    return strstr(__String_cPtr(string, 0), str);
}

char XCL_API
String_at(const String* string, const int pos)
{
    return *__String_cPtr(string, pos);
}

String XCL_API
String_dup(const String* string)
{
    return String_dupRegion(string, 0, String_size(string));
}

String XCL_API
String_dupRegion(const String* string, const int pos, const int len)
{
    String copy;
    __String_init(&copy, len);
    memcpy(__String_ptr(&copy, 0), __String_cPtr(string, pos), len);
    __String_setSize(&copy, len);
    return copy;
}
