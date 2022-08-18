#pragma once

#include <string.h>
#include <lang/XclDef.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef __TCSTR

#ifdef _UNICODE
#define __str_dispatch__(name) wcs##name
#define __TCSTR(x)             L##x
#else
#define __str_dispatch__(name) str##name
#define __TCSTR(x)             x
#endif

#undef _tcslen
#undef _tcsstr
#undef _tcschr
#undef _tcsrchr
#undef _tcscmp
#undef _tcsncmp
#undef _tcscpy
#undef _tcsncpy
#undef _tcscat
#undef _tcsdup

#define _tcslen  __str_dispatch__(len)
#define _tcsstr  __str_dispatch__(str)
#define _tcschr  __str_dispatch__(chr)
#define _tcsrchr __str_dispatch__(rchr)
#define _tcscmp  __str_dispatch__(cmp)
#define _tcsncmp __str_dispatch__(ncmp)
#define _tcscpy  __str_dispatch__(cpy)
#define _tcsncpy __str_dispatch__(ncpy)
#define _tcscat  __str_dispatch__(cat)
#define _tcsdup  __str_dispatch__(Dup)

XCL_EXPORT char* XCL_API
strDup(const char* str);

XCL_EXPORT wchar_t* XCL_API
wcsDup(const wchar_t* str);

#ifdef __cplusplus
}
#endif