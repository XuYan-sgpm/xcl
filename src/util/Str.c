#include "util/Str.h"
#include <stdlib.h>

XCL_EXPORT char* XCL_API
strDup(const char* str)
{
    if (!str)
    {
        return NULL;
    }
    size_t len = strlen(str);
    char* dup = malloc(len + 1);
    if (!dup)
    {
        return NULL;
    }
    strncpy(dup, str, len);
    dup[len] = 0;
    return dup;
}

XCL_EXPORT wchar_t* XCL_API
wcsDup(const wchar_t* str)
{
    if (!str)
    {
        return NULL;
    }
    size_t len = wcslen(str);
    wchar_t* dup = malloc((len + 1) << 1);
    if (!dup)
    {
        return NULL;
    }
    wcsncpy(dup, str, len);
    dup[len] = 0;
    return dup;
}
