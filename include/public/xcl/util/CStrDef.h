#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNICODE
#define __str_dispatch__(name) wcs##name
#else
#define __str_dispatch__(name) str##name
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

#define _tcslen  __str_dispatch__(len)
#define _tcsstr  __str_dispatch__(str)
#define _tcschr  __str_dispatch__(chr)
#define _tcsrchr __str_dispatch__(rchr)
#define _tcscmp  __str_dispatch__(cmp)
#define _tcsncmp __str_dispatch__(ncmp)
#define _tcscpy  __str_dispatch__(cpy)
#define _tcsncpy __str_dispatch__(ncpy)
#define _tcscat  __str_dispatch__(cat)

#ifdef __cplusplus
}
#endif