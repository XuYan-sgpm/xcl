//
// Created by 14049 on 2024/4/1.
//

#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <xcl/io/path.h>
#include <xcl/util/string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_VAR_PATH "${path}"
#define LOG_VAR_LEVEL "${level}"
#define LOG_VAR_FUNC "${func}"
#define LOG_VAR_FILE_NAME "${filename}"
#define LOG_VAR_TAG "${tag}"
#define LOG_VAR_LINE "${line}"
#define LOG_VAR_YEAR "${year}"
#define LOG_VAR_MONTH "${month}"
#define LOG_VAR_DAY "${day}"
#define LOG_VAR_HOUR "${hour}"
#define LOG_VAR_MINUTE "${minute}"
#define LOG_VAR_SECOND "${second}"
#define LOG_VAR_DATETIME "${datetime}"
#define LOG_VAR_MSG "${message}"

#define MAX_LOG_TAG 16
#define MAX_LOG_VAR 16

typedef enum {
  LOG_VERBOSE = 0,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
} LogLevel;

typedef struct {
  const char* tag;
  const char* log_fmt;
  const char* file_fmt;
  int show_level, write_level;
} LogConfig;

typedef struct LogFilter LogFilter;
struct LogFilter {
  struct {
    bool (*scan)(LogFilter*, const char*);
  }* __vptr;
};

typedef struct {
  long single_log_limit;
  long max_logs;
  long total_log_limit;
  long log_validity;
  LogFilter* filter;
  PathHandler* discard_cb;
} LogManageConfig;

void Log_config(LogConfig log_config, LogManageConfig log_manage_cfg);

/**
 * @brief 输出日志
 *
 * @param obj 日志对象
 * @param level 日志级别
 * @param file 文件路径
 * @param line 文件行号
 * @param func 函数名
 * @param fmt 格式化字符串
 * @param ... 可变参数
 */
void Log_log(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...);

/**
 * @brief 输出日志，和Log_log类似，但是使用va_list作为可变参数
 *
 * @param obj 日志对象
 * @param level 日志级别
 * @param file 文件路径
 * @param line 文件行号
 * @param func 函数名
 * @param fmt 格式化字符串
 * @param ap 可变参数
 */
void Log_vlog(LogLevel level, const char* file, int line, const char* func, const char* fmt,
              va_list ap);

/**
 * @brief 输出日志到控制台
 *
 * @param tag 日志标签（用于过滤日志）
 * @param level 日志级别
 * @param file 文件路径
 * @param line 文件行号
 * @param func 函数名
 * @param fmt 格式化字符串
 * @param ... 可变参数
 */
void Log_print(LogLevel level, const char* file, int line, const char* func, const char* fmt, ...);

/**
 * @brief 输出日志到控制台，使用va_list作为可变参数
 *
 * @param tag 日志标签
 * @param level 日志级别
 * @param file 文件路径
 * @param line 文件行号
 * @param func 函数名
 * @param fmt 格式化字符串
 * @param ap 可变参数
 */
void Log_vprint(LogLevel level, const char* file, int line, const char* func, const char* fmt,
                va_list ap);

/**
 * @brief log assert
 *
 * @param log log object
 * @param pred predication, skip if true; else log error msg and abort
 * @param file code file path
 * @param line code line
 * @param func function name
 * @param s format string
 * @param ... ap
 */
void Log_assert(bool pred, const char* file, int line, const char* func, const char* s, ...);

/**
 * @brief same as log_assert, but log to console
 *
 * @param pred predication
 * @param tag log tag
 * @param file file path
 * @param line code line
 * @param func function name
 * @param s format string
 * @param ... ap
 */
void Log_assert_p(bool pred, const char* file, int line, const char* func, const char* s, ...);

#define DO_LOG(level, ...) Log_log(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_V(...) DO_LOG(LOG_VERBOSE, __VA_ARGS__)
#define LOG_I(...) DO_LOG(LOG_INFO, __VA_ARGS__)
#define LOG_D(...) DO_LOG(LOG_DEBUG, __VA_ARGS__)
#define LOG_W(...) DO_LOG(LOG_WARNING, __VA_ARGS__)
#define LOG_E(...) DO_LOG(LOG_ERROR, __VA_ARGS__)
#define LOG_P(level, ...) Log_print(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOG_ASSERT_MSG(pred, ...) Log_assert(pred, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ASSERT(pred) LOG_ASSERT_MSG(pred, #pred)
#define LOG_ASSERT_PMSG(pred, ...) Log_assert_p(pred, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ASSERT_P(pred) LOG_ASSERT_PMSG(pred, #pred)

#ifdef __cplusplus
}
#endif
