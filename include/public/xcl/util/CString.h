// #include <stdint.h>
// #include <util/CBuffer.h>

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/XclDef.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    union
    {
        struct {
            char data[16];
        } stack;
        struct {
            char* ptr;
            int cap;
        } heap;
    } mem;
    uint32_t state;
} CString;

XCL_PUBLIC(CString)
String_make(const char* str);

XCL_PUBLIC(CString)
String_newRegion(const char* str, int len);

XCL_PUBLIC(CString)
String_alloc(int cap);

XCL_PUBLIC(void)
String_release(CString* string);

XCL_PUBLIC(void)
String_clear(CString* string);

XCL_PUBLIC(int)
String_size(const CString* string);

XCL_PUBLIC(int)
String_cap(const CString* string);

XCL_PUBLIC(bool)
String_empty(const CString* string);

XCL_PUBLIC(bool)
String_assign(CString* string, const char* str);

XCL_PUBLIC(bool)
String_assignRegion(CString* string, const char* str, int len);

XCL_PUBLIC(bool)
String_assignStr(CString* string, const CString* src);

XCL_PUBLIC(bool)
String_pushChar(CString* string, char ch);

XCL_PUBLIC(bool)
String_pushChars(CString* string, int n, char ch);

XCL_PUBLIC(bool)
String_pushRegion(CString* string, const char* str, int len);

XCL_PUBLIC(bool)
String_push(CString* string, const char* str);

XCL_PUBLIC(bool)
String_pushStr(CString* string, const CString* src);

XCL_PUBLIC(bool)
String_writeChar(CString* string, int pos, char ch);

XCL_PUBLIC(bool)
String_writeChars(CString* string, int pos, int n, char ch);

XCL_PUBLIC(bool)
String_writeRegion(CString* string, int pos, const char* str, int len);

XCL_PUBLIC(bool)
String_write(CString* string, int pos, const char* str);

XCL_PUBLIC(bool)
String_writeStr(CString* string, int pos, const CString* src);

XCL_PUBLIC(bool)
String_pop(CString* string, char* dst);

XCL_PUBLIC(bool)
String_popBack(CString* string, char* dst);

XCL_PUBLIC(bool)
String_get(const CString* string, int pos, char* dst);

XCL_PUBLIC(int)
String_gets(const CString* string, int pos, int len, char* dst);

XCL_PUBLIC(void)
String_delete(CString* string, int pos);

XCL_PUBLIC(void)
String_deleteRegion(CString* string, int pos, int len);

XCL_PUBLIC(const char*)
String_queryChar(const CString* string, bool left, char ch);

XCL_PUBLIC(const char*)
String_query(const CString* string, const char* str);

XCL_PUBLIC(const char*)
String_queryRegion(const CString* string, const char* str, int len);

XCL_PUBLIC(char)
String_at(const CString* string, int pos);

XCL_PUBLIC(CString)
String_dup(const CString* string);

XCL_PUBLIC(CString)
String_dupRegion(const CString* string, int pos, int len);

#ifdef __cplusplus
}
#endif