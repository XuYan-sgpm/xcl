//
// Created by 14049 on 2024/4/1.
//

#include <internal/util/log_impl.h>
#include <internal/xcl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <xcl/concurrent/util/lazy.h>
#include <xcl/util/log.h>

void Log_config(LogConfig log_config, LogManageConfig log_manage_cfg) {
  Xcl_init(XCL_LOG);
  LogImpl_config(__xcl_log, &log_config, &log_manage_cfg);
}

void Log_log(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  LogImpl_writev(__xcl_log, level, file, line, func, fmt, ap);
  va_end(ap);
}

void Log_vlog(LogLevel level, const char* file, int line, const char* func, const char* fmt,
              va_list ap) {
  LogImpl_writev(__xcl_log, level, file, line, func, fmt, ap);
}

void Log_print(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  LogImpl_vprint(__xcl_log, level, file, line, func, fmt, ap);
  va_end(ap);
}

void Log_vprint(LogLevel level, const char* file, int line, const char* func, const char* fmt,
                va_list ap) {
  LogImpl_vprint(__xcl_log, level, file, line, func, fmt, ap);
}

void Log_assert(bool pred, const char* file, int line, const char* func, const char* s, ...) {
  if (!pred) {
    va_list ap;
    va_start(ap, s);
    LogImpl_writev(__xcl_log, LOG_ERROR, file, line, func, s, ap);
    va_end(ap);
    abort();
  }
}

void Log_assert_p(bool pred, const char* file, int line, const char* func, const char* s, ...) {
  if (!pred) {
    va_list ap;
    va_start(ap, s);
    LogImpl_vprint(__xcl_log, LOG_ERROR, file, line, func, s, ap);
    va_end(ap);
    abort();
  }
}
