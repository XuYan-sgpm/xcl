// #include <stdint.h>
// #include <util/CBuffer.h>

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/XclDef.h>

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    union
    {
        struct
        {
            char data[16];
        } stack;
        struct
        {
            char*ptr;
            int cap;
        } heap;
    } mem;
    int32_t state;
} CString;

XCL_PUBLIC(CString)
String_new(const char*str);

XCL_PUBLIC(CString)
String_newRegion(const char*str, const int len);

XCL_PUBLIC(CString)
String_alloc(const int cap);

XCL_PUBLIC(void)
String_release(CString*string);

XCL_PUBLIC(void)
String_clear(CString*string);

XCL_PUBLIC(int)
String_size(const CString*string);

XCL_PUBLIC(int)
String_cap(const CString*string);

XCL_PUBLIC(bool)
String_empty(const CString*string);

XCL_PUBLIC(bool)
String_assign(CString*string, const char*str);

XCL_PUBLIC(bool)
String_assignRegion(CString*string, const char*str, const int len);

XCL_PUBLIC(bool)
String_assignStr(CString*string, const CString*src);

XCL_PUBLIC(bool)
String_pushChar(CString*string, char ch);

XCL_PUBLIC(bool)
String_pushChars(CString*string, const int n, char ch);

XCL_PUBLIC(bool)
String_pushRegion(CString*string, const char*str, const int len);

XCL_PUBLIC(bool)
String_push(CString*string, const char*str);

XCL_PUBLIC(bool)
String_pushStr(CString*string, const CString*src);

XCL_PUBLIC(bool)
String_writeChar(CString*string, const int pos, char ch);

XCL_PUBLIC(bool)
String_writeChars(CString*string, const int pos, const int n, char ch);

XCL_PUBLIC(bool)
String_writeRegion(CString*string,
                   const int pos,
                   const char*str,
                   const int len);

XCL_PUBLIC(bool)
String_write(CString*string, const int pos, const char*str);

XCL_PUBLIC(bool)
String_writeStr(CString*string, const int pos, const CString*src);

XCL_PUBLIC(bool)
String_pop(CString*string, char*dst);

XCL_PUBLIC(bool)
String_popBack(CString*string, char*dst);

XCL_PUBLIC(bool)
String_get(const CString*string, const int pos, char*dst);

XCL_PUBLIC(int)
String_gets(const CString*string, const int pos, const int len, char*dst);

XCL_PUBLIC(void)
String_delete(CString*string, const int pos);

XCL_PUBLIC(void)
String_deleteRegion(CString*string, const int pos, const int len);

XCL_PUBLIC(const char*)
String_queryChar(const CString*string, const bool left, char ch);

XCL_PUBLIC(const char*)
String_query(const CString*string, const char*str);

XCL_PUBLIC(const char*)
String_queryRegion(const CString*string, const char*str, const int len);

XCL_PUBLIC(char)
String_at(const CString*string, const int pos);

XCL_PUBLIC(CString)
String_dup(const CString*string);

XCL_PUBLIC(CString)
String_dupRegion(const CString*string, const int pos, const int len);

#ifdef __cplusplus
}
#endif