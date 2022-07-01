//
// Created by xuyan on 2022/6/30.
//

#include "lang/CAssertUtil.h"
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

void assertInfo(bool cond, const char *msg) {
  if (!cond) {
    fprintf(stderr, "%s\n", msg);
    assert(cond);
  }
}

void assertMessage(bool cond, const char *fmt, ...) {
  if (!cond) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    assert(false);
  }
}
