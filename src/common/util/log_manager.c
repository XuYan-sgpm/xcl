#include <internal/util/log_manager.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/concurrent/util/lazy.h>
#include <xcl/io/path.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/comparators.h>
#include <xcl/util/log.h>

bool postExit(void (*f)());

typedef struct {
  String path;
  int64_t ctime;
  long size;
} LogFileInfo;

static LogFileInfo LogFileInfo_make(const char* path, int64_t ctime, long size) {
  LogFileInfo log_file_info = {.ctime = ctime, .size = size, .path = String_from(path, -1)};
  LOG_ASSERT_PMSG(!String_empty(&log_file_info.path), "init log file info path failed");
  return log_file_info;
}
static const char* LogFileInfo_path(LogFileInfo* log_file_info) {
  return String_str(&log_file_info->path);
}
static void LogFileInfo_destroy(LogFileInfo* log_file_info) {
  String_destroy(&log_file_info->path);
}

static int LogFileInfo_compare(Comparator* comparator, const void* o1, const void* o2) {
  const LogFileInfo *fi1 = o1, *fi2 = o2;
  int64_t diff = fi1->ctime - fi2->ctime;
  return diff < 0 ? -1 : diff > 0 ? 1 : 0;
}
static Comparator* LogFileInfo_comparator() {
  static void* lfi_comparator_vptr[] = {LogFileInfo_compare};
  static Comparator lfi_comparator = {.__vptr = (void*)lfi_comparator_vptr};
  return &lfi_comparator;
}

static bool LogManager_filterLog(LogManager* manager, const char* name) {
  return manager->config.filter ? manager->config.filter->__vptr->scan(manager->config.filter, name)
                                : true;
}
static bool LogManager_discardLog(LogManager* manager, const char* path) {
  if (manager->config.discard_cb)
    return PathHandler_process(manager->config.discard_cb, path);
  LOG_P(LOG_DEBUG, "remove log %s", path);
  return remove(path) == 0;
}

typedef struct {
  PathHandler super;
  LogManager* manager;
} LogManageHandler;

bool LogManager_pop(LogManager* manager) {
  if (Heap_empty(manager->log_file_infos))
    return false;
  LogFileInfo log_file_info;
  Heap_top(manager->log_file_infos, &log_file_info);
  if (!LogManager_discardLog(manager, String_str(&log_file_info.path)))
    return false;
  Heap_pop(manager->log_file_infos, &log_file_info);
  manager->total_log_size -= log_file_info.size;
  LOG_P(LOG_DEBUG, "remove oldest log %s, size: %ld, total log size: %ld",
        LogFileInfo_path(&log_file_info), log_file_info.size, manager->total_log_size);
  LogFileInfo_destroy(&log_file_info);
  return true;
}
static bool LogManager_cache(LogManager* manager, const char* path, int64_t ctime, long size) {
  LogFileInfo log_file_info = LogFileInfo_make(path, ctime, size);
  if (!Heap_push(manager->log_file_infos, &log_file_info))
    return false;
  manager->total_log_size += size;
  LOG_P(LOG_DEBUG, "add log %s, log size:%ld, total log size %ld", path, size,
        manager->total_log_size);
  return true;
}
bool LogManager_add(LogManager* manager, const char* path, int64_t ctime) {
  const char* ext = strrchr(path, '.');
  if (!ext || strcmp(ext, ".log")) {
    return true;
  }
  if (!LogManager_filterLog(manager, path)) {
    LOG_P(LOG_DEBUG, "filter log %s failed, pass", path);
    return true;
  }
  LogManageConfig* config = &manager->config;
  if (config->max_logs == 0 || config->total_log_limit == 0 || config->log_validity == 0 ||
      config->single_log_limit == 0) {
    LOG_P(LOG_DEBUG, "no log file allowed");
    return LogManager_discardLog(manager, path);
  }
  struct stat st;
  if (stat(path, &st)) {
    LOG_P(LOG_ERROR, "stat %s failed", path);
    return false;
  }
  time_t now = time(NULL);
  if (config->log_validity > 0 && now - st.st_ctime > config->log_validity) {
    LOG_P(LOG_ERROR, "log %s expire, duration %ld", path, now - st.st_ctime);
    return LogManager_discardLog(manager, path);
  }
  if (config->single_log_limit > 0 && st.st_size > config->single_log_limit) {
    LOG_P(LOG_DEBUG, "log %s file size %ld > single log limit %ld", path, st.st_size,
          config->single_log_limit);
    return LogManager_discardLog(manager, path);
  }
  if (config->total_log_limit > 0 && st.st_size > config->total_log_limit) {
    LOG_P(LOG_ERROR, "log %s size %ld > total log limit %ld", path, st.st_size,
          config->total_log_limit);
    return LogManager_discardLog(manager, path);
  }
  if (config->max_logs > 0 && Heap_size(manager->log_file_infos) == config->max_logs &&
      !LogManager_pop(manager)) {
    return false;
  }
  while (config->total_log_limit > 0 &&
         manager->total_log_size + st.st_size > config->total_log_limit) {
    LOG_P(LOG_DEBUG, "total log limit exceed, %ld+%ld>%ld", manager->total_log_size, st.st_size,
          config->total_log_limit);
    if (!LogManager_pop(manager))
      return false;
  }
  if (manager->log_file_infos &&
      !LogManager_cache(manager, path, ctime > 0 ? ctime : st.st_ctime * 1000, st.st_size)) {
    LOG_P(LOG_ERROR, "add log %s failed", path);
    return false;
  }
  return true;
}

static bool LogManageHandler_process(PathHandler* cb, const char* path) {
  LogManageHandler* log_manage_handler = (void*)cb;
  return LogManager_add(log_manage_handler->manager, path, -1);
}
static LogManageHandler LogManageHandler_make(LogManager* manager) {
  static void* const log_manage_handler_vptr[] = {LogManageHandler_process};
  LogManageHandler o = {.manager = manager, .super.__vptr = (void*)log_manage_handler_vptr};
  return o;
}
LogManager* LogManager_new(LogManageConfig config) {
  LogManager* manager = palloc(sizeof(LogManager));
  if (manager && LogManager_init(manager, config))
    return manager;
  pfree(manager);
  return NULL;
}
void LogManager_free(LogManager* manager) {
  if (manager) {
    LogManager_destroy(manager);
    pfree(manager);
  }
}
bool LogManager_init(LogManager* manager, LogManageConfig config) {
  if (config.total_log_limit >= 0 && config.single_log_limit >= 0 &&
      config.total_log_limit < config.single_log_limit) {
    LOG_P(LOG_DEBUG, "total log limit %d < single log limit %d", config.total_log_limit,
          config.single_log_limit);
    return false;
  }
  manager->total_log_size = -1;
  manager->config = config;
  if (config.max_logs <= 0 && config.total_log_limit <= 0)
    return manager->log_file_infos = NULL, true;
  manager->log_file_infos = Heap_new(sizeof(LogFileInfo), LogFileInfo_comparator());
  LOG_ASSERT_P(manager->log_file_infos);
  manager->total_log_size = 0;
  return true;
}
void LogManager_clear(LogManager* manager) {
  if (manager && manager->log_file_infos) {
    Heap* heap = manager->log_file_infos;
    while (!Heap_empty(heap)) {
      LogFileInfo log_file_info;
      Heap_pop(heap, &log_file_info);
      LogFileInfo_destroy(&log_file_info);
    }
    manager->total_log_size = 0;
  }
}
void LogManager_destroy(LogManager* manager) {
  if (manager) {
    LogManager_clear(manager);
    Heap_free(manager->log_file_infos);
    memset(manager, 0, sizeof(*manager));
  }
}
bool LogManager_exec(LogManager* manager, const char* cwd, bool recursive) {
  LogManageHandler log_manage_handler = LogManageHandler_make(manager);
  return (recursive ? Path_walk : Path_listDir)(cwd, &log_manage_handler.super);
}
long LogManager_currWriteLimit(LogManager* manager) {
  if (manager->config.total_log_limit < 0 && manager->config.single_log_limit < 0)
    return -1;
  if (manager->config.total_log_limit < 0)
    return manager->config.single_log_limit;
  long bytes = manager->config.total_log_limit - manager->total_log_size;
  if (manager->config.single_log_limit < 0)
    return bytes;
  return bytes > manager->config.single_log_limit ? manager->config.single_log_limit : bytes;
}
long LogManager_window(LogManager* manager) {
  return manager->log_file_infos ? Heap_size(manager->log_file_infos) : -1;
}
