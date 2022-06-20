// #include <stdint.h>
// #include <util/CBuffer.h>

#include <stdbool.h>

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
  int state;
} CString;

CString makeString(const char *str);

CString makeString2(const char *str, const int len);

CString strAlloc(const int cap);

void strRelease(CString *string);

void strClear(CString *string);

int strSize(const CString *string);

int strCap(const CString *string);

bool strEmpty(const CString *string);

bool strAssign(CString *string, const char *str);

bool strAssignRegion(CString *string, const char *str, const int len);

bool strAssignStr(CString *string, const CString *src);

bool strPushChar(CString *string, char ch);

bool strPushChars(CString *string, const int n, char ch);

bool strPushRegion(CString *string, const char *str, const int len);

bool strPush(CString *string, const char *str);

bool strPushStr(CString *string, const CString *src);

bool strWriteChar(CString *string, const int pos, char ch);

bool strWriteChars(CString *string, const int pos, const int n, char ch);

bool strWriteRegion(CString *string,
                    const int pos,
                    const char *str,
                    const int len);

bool strWrite(CString *string, const int pos, const char *str);

bool strWriteStr(CString *string, const int pos, const CString *src);

bool strPop(CString *string, char *dst);

bool strPopBack(CString *string, char *dst);

bool strGet(const CString *string, const int pos, char *dst);

int strGets(const CString *string, const int pos, const int len, char *dst);

void strDelete(CString *string, const int pos);

void strDeleteRegion(CString *string, const int pos, const int len);

const char *strQueryChar(const CString *string, const bool left, char ch);

const char *strQuery(const CString *string, const char *str);

const char *
strQueryRegion(const CString *string, const char *str, const int len);

char strAt(const CString *string, const int pos);

CString strDup(const CString *string);

CString strDupRegion(const CString *string, const int pos, const int len);

#ifdef __cplusplus
}
#endif