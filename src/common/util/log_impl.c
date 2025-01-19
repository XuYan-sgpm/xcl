#include <internal/util/log_format.h>
#include <internal/util/log_impl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/util/string.h>

#define MAX_LOG_BUFFER_SIZE 4096

#if defined(Windows)
#include <windows.h>
#elif defined(Android)
#include <android/log.h>
#endif

static const char* default_log_fmt =
    "${datetime} [${tag}] [${level}] ${filename}:${line} ${function} "
    "${message}";

static void LogImpl_closeWrite(LogImpl* log_impl);
void LogImpl_destroy(LogImpl* log_impl) {
  Mutex_delete(log_impl->mut);
  String_destroy(&log_impl->log_fmt);
  if (log_impl->write_level >= LOG_VERBOSE && log_impl->write_level <= LOG_ERROR)
    LogImpl_closeWrite(log_impl);
}

void LogImpl_init(LogImpl* log_impl) {
  memset(log_impl, 0, sizeof(*log_impl));
  log_impl->log_fmt = String_from(default_log_fmt, -1);
  snprintf(log_impl->tag, sizeof(log_impl->tag), "%s", "null");
  log_impl->print_level = LOG_VERBOSE;
  log_impl->mut = Mutex_new();
  log_impl->write_level = -1;
}

static inline bool LogImpl_isLevelValid(unsigned level) {
  return level >= LOG_VERBOSE && level <= LOG_ERROR;
}

static void LogImpl_prints(const char* tag, const char* s, unsigned s_len, LogLevel level) {
#ifdef Android
  static const int android_log_levels[] = {ANDROID_LOG_VERBOSE, ANDROID_LOG_DEBUG,
                                           ANDROID_LOG_INFO,    ANDROID_LOG_WARN,
                                           ANDROID_LOG_ERROR,   ANDROID_LOG_FATAL};
  __android_log_print(android_log_levels[level], tag, "%s\n", s);
#else
#if defined(Windows) && defined(_Debug)
  OutputDebugString(s);
#endif
  FILE* fp = level < LOG_WARNING ? stdout : stderr;
  fwrite(s, 1, s_len, fp);
  fflush(fp);
#endif
}

static LogCtx LogImpl_makeCtx(LogImpl* log_impl, LogLevel level, const char* file, int line,
                              const char* func, const char* fmt, va_list ap) {
  time_t now = time(NULL);
  struct _timeval tv = {0};
  _gettimeofday(&tv, NULL);
  LogCtx log_ctx = {.msg_fmt = fmt,
                    .func = func,
                    .file = file,
                    .line = line,
                    .tag = log_impl->tag,
                    .level = level,
                    .time_stamp = *localtime(&now),
                    .ms = tv.tv_usec / 1000};
  va_copy(log_ctx.msg_args, ap);
  return log_ctx;
}

static void LogImpl_checkConfig(LogConfig* config) {
  if (!config->tag || !config->tag[0])
    config->tag = "null";
  if (!config->file_fmt)
    config->file_fmt = "";
  if (!config->log_fmt)
    config->log_fmt = "";
}

static bool LogImpl_skip(LogImpl* log_impl, int level) {
  return !LogImpl_isLevelValid(level) ||
         (level < log_impl->write_level && level < log_impl->print_level);
}

static unsigned LogImpl_build(LogImpl* log_impl, LogCtx* log_ctx, char* out, unsigned out_len) {
  unsigned limit = out_len - 2;
  const char* fmt = String_str(&log_impl->log_fmt);
  unsigned size = Log_format(fmt, log_ctx, out, limit + 1);
  if (size > limit)
    size = limit;
  out[size++] = '\n';
  return size;
}

void LogImpl_writev(LogImpl* log_impl, LogLevel level, const char* file, int line, const char* func,
                    const char* fmt, va_list ap) {
  if (!LogImpl_skip(log_impl, level)) {
    LogCtx log_ctx = LogImpl_makeCtx(log_impl, level, file, line, func, fmt, ap);
    char out[MAX_LOG_BUFFER_SIZE + 2] = {0};
    unsigned out_size = LogImpl_build(log_impl, &log_ctx, out, sizeof(out));
    Mutex_lock(log_impl->mut);
    if (log_impl->write_level <= log_ctx.level)
      LogFileWriter_write(&log_impl->writer, out, out_size, &log_ctx);
    if (log_ctx.level >= log_impl->print_level)
      LogImpl_prints(log_ctx.tag, out, out_size, log_ctx.level);
    Mutex_unlock(log_impl->mut);
    va_end(log_ctx.msg_args);
  }
}

void LogImpl_vprint(LogImpl* log_impl, LogLevel level, const char* file, int line, const char* func,
                    const char* fmt, va_list ap) {
  if (LogImpl_isLevelValid(level) && level >= log_impl->print_level) {
    LogCtx log_ctx = LogImpl_makeCtx(log_impl, level, file, line, func, fmt, ap);
    char out[MAX_LOG_BUFFER_SIZE + 2] = {0};
    unsigned out_size = LogImpl_build(log_impl, &log_ctx, out, sizeof(out));
    Mutex_lock(log_impl->mut);
    LogImpl_prints(log_ctx.tag, out, out_size, log_ctx.level);
    Mutex_unlock(log_impl->mut);
    va_end(log_ctx.msg_args);
  }
}

static void LogImpl_closeWrite(LogImpl* log_impl) {
  LogManager* manager = log_impl->writer.manager;
  LogFileWriter_destroy(&log_impl->writer);
  LogManager_free(manager);
}

static bool LogImpl_isNeedManager(LogManageConfig* log_manage_cfg) {
  return log_manage_cfg &&
         (log_manage_cfg->single_log_limit >= 0 || log_manage_cfg->total_log_limit >= 0 ||
          log_manage_cfg->max_logs >= 0 || log_manage_cfg->log_validity >= 0);
}

static bool LogImpl_initWriter(LogImpl* log_impl, const char* file_fmt,
                               LogManageConfig* log_manage_cfg) {
  LOG_P(LOG_DEBUG, "initialize log file writer");
  LogManager* manager = NULL;
  if (LogImpl_isNeedManager(log_manage_cfg) && !(manager = LogManager_new(*log_manage_cfg)))
    return false;
  if (LogFileWriter_init(&log_impl->writer, file_fmt, manager))
    return true;
  LogManager_free(manager);
  return false;
}

static bool LogImpl_compareManager(LogImpl* log_impl, LogManageConfig* log_manage_cfg) {
  if (!log_impl->writer.manager)
    return !LogImpl_isNeedManager(log_manage_cfg);
  if (!LogImpl_isNeedManager(log_manage_cfg))
    return false;
  return memcmp(&log_impl->writer.manager->config, log_manage_cfg, sizeof(*log_manage_cfg)) == 0;
}

static bool LogImpl_compareLogPrefix(LogImpl* log_impl, const char* file_fmt) {
  char log_prefix[PATH_LIMIT];
  Path_dirname_r(file_fmt, log_prefix, sizeof(log_prefix));
  return strcmp(log_prefix, String_str(&log_impl->writer.dir)) == 0;
}

static bool LogImpl_configWriter(LogImpl* log_impl, LogConfig* log_cfg,
                                 LogManageConfig* log_manage_cfg) {
  if (!LogImpl_isLevelValid(log_cfg->write_level)) {
    if (LogImpl_isLevelValid(log_impl->write_level))
      LogImpl_closeWrite(log_impl);
    return true;
  }
  if (!log_cfg->file_fmt || !log_cfg->file_fmt[0])
    return false;
  if (!LogImpl_isLevelValid(log_impl->write_level))
    return LogImpl_initWriter(log_impl, log_cfg->file_fmt, log_manage_cfg);
  if (!LogImpl_compareManager(log_impl, log_manage_cfg) ||
      !LogImpl_compareLogPrefix(log_impl, log_cfg->file_fmt)) {
    LogImpl_closeWrite(log_impl);
    return LogImpl_initWriter(log_impl, log_cfg->file_fmt, log_manage_cfg);
  }
  char name_fmt[MAX_PATH_NAME];
  Path_basename_r(log_cfg->file_fmt, name_fmt, sizeof(name_fmt));
  if (strcmp(name_fmt, String_str(&log_impl->writer.name_fmt)))
    LogFileWriter_changeNameFmt(&log_impl->writer, name_fmt);
  return true;
}

bool LogImpl_config(LogImpl* log_impl, LogConfig* log_cfg, LogManageConfig* log_manage_cfg) {
  LogImpl_checkConfig(log_cfg);
  if (!log_impl->mut && !(log_impl->mut = Mutex_new()))
    return false;
  if (LogImpl_isLevelValid(log_cfg->write_level) &&
      !LogImpl_configWriter(log_impl, log_cfg, log_manage_cfg))
    return false;
  log_impl->write_level = log_cfg->write_level;
  snprintf(log_impl->tag, sizeof(log_impl->tag), "%s", log_cfg->tag);
  String_fill(&log_impl->log_fmt, log_cfg->log_fmt[0] ? log_cfg->log_fmt : default_log_fmt, -1);
  log_impl->print_level = log_cfg->show_level;
  return true;
}
