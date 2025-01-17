#pragma once

#include <internal/util/log_format.h>
#include <internal/util/log_manager.h>
#include <stdio.h>
#include <time.h>
#include <xcl/util/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  LogManager* manager;
  String name_fmt;
  FILE* fp;
  String name;
  String dir;
  long curr_write_limit;
  struct tm last_check_point;
  int64_t curr_fp_ctime;
} LogFileWriter;

bool LogFileWriter_init(LogFileWriter* writer, const char* log_file_fmt, LogManager* manager);
void LogFileWriter_destroy(LogFileWriter* writer);
bool LogFileWriter_write(LogFileWriter* writer, const void* buf, unsigned len, LogCtx* log_ctx);
void LogFileWriter_changeNameFmt(LogFileWriter* writer, const char* name_fmt);

#ifdef __cplusplus
}
#endif
