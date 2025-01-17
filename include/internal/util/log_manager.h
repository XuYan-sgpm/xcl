#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <xcl/io/path.h>
#include <xcl/util/heap.h>
#include <xcl/util/log.h>
#include <xcl/util/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  Heap* log_file_infos;
  LogManageConfig config;
  long total_log_size;
} LogManager;

bool LogManager_init(LogManager* manager, LogManageConfig config);
void LogManager_destroy(LogManager* manager);
LogManager* LogManager_new(LogManageConfig config);
void LogManager_free(LogManager* manager);
bool LogManager_add(LogManager* manager, const char* path, int64_t ctime);
bool LogManager_pop(LogManager* manager);
bool LogManager_exec(LogManager* manager, const char* cwd, bool recursive);
void LogManager_clear(LogManager* manager);
long LogManager_currWriteLimit(LogManager* manager);
long LogManager_window(LogManager* manager);

#ifdef __cplusplus
}
#endif
