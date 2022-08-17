//
// Created by xuyan on 2022/6/30.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "xcldef.h"
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

XCL_EXPORT void XCL_API
assertIf(bool cond, const char* msg)
{
    if (!cond)
    {
        fprintf(stderr, "%s\n", msg);
        assert(cond);
    }
}

XCL_EXPORT void
assertMessage(bool cond, const char* fmt, ...)
{
    if (!cond)
    {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fputc('\n', stderr);
        assert(false);
    }
}

#ifdef __cplusplus
}
#endif
