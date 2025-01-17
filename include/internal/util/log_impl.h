#pragma once

#include <xcl/util/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <internal/util/log_file_writer.h>
typedef struct {
  void* mut;
  char tag[MAX_LOG_TAG];
  String log_fmt;
  LogFileWriter writer;
  unsigned write_level;
  unsigned print_level;
} LogImpl;

void LogImpl_destroy(LogImpl* log_impl);
void LogImpl_init(LogImpl* log_impl);
void LogImpl_writev(LogImpl* log_impl, LogLevel level, const char* file, int line, const char* func,
                    const char* fmt, va_list ap);
void LogImpl_vprint(LogImpl* log_impl, LogLevel level, const char* file, int line, const char* func,
                    const char* fmt, va_list ap);
bool LogImpl_config(LogImpl* log_impl, LogConfig* log_config, LogManageConfig* log_manage_config);

#ifdef __cplusplus
}
#endif
