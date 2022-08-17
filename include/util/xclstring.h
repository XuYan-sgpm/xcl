#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <lang/xcldef.h>

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

XCL_EXPORT CString XCL_API
String_make(const char* str);

XCL_EXPORT CString XCL_API
String_newRegion(const char* str, int len);

XCL_EXPORT CString XCL_API
String_alloc(int cap);

XCL_EXPORT void XCL_API
String_release(CString* string);

XCL_EXPORT void XCL_API
String_clear(CString* string);

XCL_EXPORT int XCL_API
String_size(const CString* string);

XCL_EXPORT int XCL_API
String_cap(const CString* string);

XCL_EXPORT bool XCL_API
String_empty(const CString* string);

XCL_EXPORT bool XCL_API
String_assign(CString* string, const char* str);

XCL_EXPORT bool XCL_API
String_assignRegion(CString* string, const char* str, int len);

XCL_EXPORT bool XCL_API
String_assignStr(CString* string, const CString* src);

XCL_EXPORT bool XCL_API
String_pushChar(CString* string, char ch);

XCL_EXPORT bool XCL_API
String_pushChars(CString* string, int n, char ch);

XCL_EXPORT bool XCL_API
String_pushRegion(CString* string, const char* str, int len);

XCL_EXPORT bool XCL_API
String_push(CString* string, const char* str);

XCL_EXPORT bool XCL_API
String_pushStr(CString* string, const CString* src);

XCL_EXPORT bool XCL_API
String_writeChar(CString* string, int pos, char ch);

XCL_EXPORT bool XCL_API
String_writeChars(CString* string, int pos, int n, char ch);

XCL_EXPORT bool XCL_API
String_writeRegion(CString* string, int pos, const char* str, int len);

XCL_EXPORT bool XCL_API
String_write(CString* string, int pos, const char* str);

XCL_EXPORT bool XCL_API
String_writeStr(CString* string, int pos, const CString* src);

XCL_EXPORT bool XCL_API
String_pop(CString* string, char* dst);

XCL_EXPORT bool XCL_API
String_popBack(CString* string, char* dst);

XCL_EXPORT bool XCL_API
String_get(const CString* string, int pos, char* dst);

XCL_EXPORT int XCL_API
String_gets(const CString* string, int pos, int len, char* dst);

XCL_EXPORT void XCL_API
String_delete(CString* string, int pos);

XCL_EXPORT void XCL_API
String_deleteRegion(CString* string, int pos, int len);

XCL_EXPORT const char* XCL_API
String_queryChar(const CString* string, bool left, char ch);

XCL_EXPORT const char* XCL_API
String_query(const CString* string, const char* str);

XCL_EXPORT char XCL_API
String_at(const CString* string, int pos);

XCL_EXPORT CString XCL_API
String_dup(const CString* string);

XCL_EXPORT CString XCL_API
String_dupRegion(const CString* string, int pos, int len);

#ifdef __cplusplus
}
#endif