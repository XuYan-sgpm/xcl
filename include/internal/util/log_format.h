//
// Created by 14049 on 2024/6/20.
//

#pragma once

#include <stdarg.h>
#include <time.h>
#include <xcl/util/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const char* file;
  const char* func;
  int line;
  const char* msg_fmt;
  va_list msg_args;
  const char* tag;
  unsigned level;
  struct tm time_stamp;
  int ms;
} LogCtx;

unsigned Log_format(const char* fmt, LogCtx* log_ctx, char* out, unsigned out_len);

#ifdef __cplusplus
}
#endif
