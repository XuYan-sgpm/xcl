#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/xcl_def.h>

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
} String;

XCL_EXPORT String XCL_API
String_make(const char* str);

XCL_EXPORT String XCL_API
String_newRegion(const char* str, int len);

XCL_EXPORT String XCL_API
String_alloc(int cap);

XCL_EXPORT void XCL_API
String_release(String* string);

XCL_EXPORT void XCL_API
String_clear(String* string);

XCL_EXPORT int XCL_API
String_size(const String* string);

XCL_EXPORT int XCL_API
String_cap(const String* string);

XCL_EXPORT bool XCL_API
String_empty(const String* string);

XCL_EXPORT bool XCL_API
String_assign(String* string, const char* str);

XCL_EXPORT bool XCL_API
String_assignRegion(String* string, const char* str, int len);

XCL_EXPORT bool XCL_API
String_assignStr(String* string, const String* src);

XCL_EXPORT bool XCL_API
String_pushChar(String* string, char ch);

XCL_EXPORT bool XCL_API
String_pushChars(String* string, int n, char ch);

XCL_EXPORT bool XCL_API
String_pushRegion(String* string, const char* str, int len);

XCL_EXPORT bool XCL_API
String_push(String* string, const char* str);

XCL_EXPORT bool XCL_API
String_pushStr(String* string, const String* src);

XCL_EXPORT bool XCL_API
String_writeChar(String* string, int pos, char ch);

XCL_EXPORT bool XCL_API
String_writeChars(String* string, int pos, int n, char ch);

XCL_EXPORT bool XCL_API
String_writeRegion(String* string, int pos, const char* str, int len);

XCL_EXPORT bool XCL_API
String_write(String* string, int pos, const char* str);

XCL_EXPORT bool XCL_API
String_writeStr(String* string, int pos, const String* src);

XCL_EXPORT bool XCL_API
String_pop(String* string, char* dst);

XCL_EXPORT bool XCL_API
String_popBack(String* string, char* dst);

XCL_EXPORT bool XCL_API
String_get(const String* string, int pos, char* dst);

XCL_EXPORT int XCL_API
String_gets(const String* string, int pos, int len, char* dst);

XCL_EXPORT void XCL_API
String_delete(String* string, int pos);

XCL_EXPORT void XCL_API
String_deleteRegion(String* string, int pos, int len);

XCL_EXPORT const char* XCL_API
String_queryChar(const String* string, bool left, char ch);

XCL_EXPORT const char* XCL_API
String_query(const String* string, const char* str);

XCL_EXPORT char XCL_API
String_at(const String* string, int pos);

XCL_EXPORT String XCL_API
String_dup(const String* string);

XCL_EXPORT String XCL_API
String_dupRegion(const String* string, int pos, int len);

#ifdef __cplusplus
}
#endif