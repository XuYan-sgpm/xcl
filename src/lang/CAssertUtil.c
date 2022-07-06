//
// Created by xuyan on 2022/6/30.
//

#include "lang/CAssertUtil.h"
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

XCL_PUBLIC void XCL_API assertIf(bool cond, const char *msg) {
  if (!cond) {
    fprintf(stderr, "%s\n", msg);
    assert(cond);
  }
}

XCL_PUBLIC void assertMessage(bool cond, const char *fmt, ...) {
  if (!cond) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    assert(false);
  }
}
