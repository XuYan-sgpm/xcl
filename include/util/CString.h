// #include <stdint.h>
// #include <util/CBuffer.h>

#include <stdbool.h>
#include <stdint.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  union {
    struct {
      char data[16];
    } stack;
    struct {
      char *ptr;
      int cap;
    } heap;
  } mem;
  int32_t state;
} CString;

CString String_new(const char *str);

CString String_newRegion(const char *str, const int len);

CString String_alloc(const int cap);

void String_release(CString *string);

void String_clear(CString *string);

int String_size(const CString *string);

int String_cap(const CString *string);

bool String_empty(const CString *string);

bool String_assign(CString *string, const char *str);

bool String_assignRegion(CString *string, const char *str, const int len);

bool String_assignStr(CString *string, const CString *src);

bool String_pushChar(CString *string, char ch);

bool String_pushChars(CString *string, const int n, char ch);

bool String_pushRegion(CString *string, const char *str, const int len);

bool String_push(CString *string, const char *str);

bool String_pushStr(CString *string, const CString *src);

bool String_writeChar(CString *string, const int pos, char ch);

bool String_writeChars(CString *string, const int pos, const int n, char ch);

bool String_writeRegion(CString *string,
                        const int pos,
                        const char *str,
                        const int len);

bool String_write(CString *string, const int pos, const char *str);

bool String_writeStr(CString *string, const int pos, const CString *src);

bool String_pop(CString *string, char *dst);

bool String_popBack(CString *string, char *dst);

bool String_get(const CString *string, const int pos, char *dst);

int String_gets(const CString *string, const int pos, const int len, char *dst);

void String_delete(CString *string, const int pos);

void String_deleteRegion(CString *string, const int pos, const int len);

const char *String_queryChar(const CString *string, const bool left, char ch);

const char *String_query(const CString *string, const char *str);

const char *
String_queryRegion(const CString *string, const char *str, const int len);

char String_at(const CString *string, const int pos);

CString String_dup(const CString *string);

CString String_dupRegion(const CString *string, const int pos, const int len);

#ifdef __cplusplus
}
#endif