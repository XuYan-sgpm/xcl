//
// Created by 14049 on 2024/6/20.
//

#include <internal/util/log_format.h>
#include <stdio.h>
#include <string.h>
#include <xcl/io/path.h>
#include <xcl/util/log.h>

const static char* level_names[] = {"verbose", "debug", "info", "warning", "error", "fatal"};
static const char* log_var_names[] = {"path",   "level",  "function", "filename", "tag",
                                      "line",   "year",   "month",    "day",      "hour",
                                      "minute", "second", "datetime", "message",  NULL};

static unsigned evalPath(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%s", log_ctx->file);
}
static unsigned evalLevel(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%s", level_names[log_ctx->level]);
}
static unsigned evalFunc(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%s", log_ctx->func);
}
static unsigned evalFilename(LogCtx* log_ctx, char* out, unsigned out_len) {
  const char* sep = strrchr(log_ctx->file, '/');
#ifdef Windows
  if (!sep)
    sep = strrchr(log_ctx->file, '\\');
#endif
  return snprintf(out, out_len, "%s", sep ? ++sep : "null");
}
static unsigned evalTag(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%s", log_ctx->tag);
}
static unsigned evalLine(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%d", log_ctx->line);
}
static unsigned evalYear(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%04d", log_ctx->time_stamp.tm_year + 1900);
}
static unsigned evalMonth(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%02d", log_ctx->time_stamp.tm_mon + 1);
}
static unsigned evalDay(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%02d", log_ctx->time_stamp.tm_mday);
}
static unsigned evalHour(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%02d", log_ctx->time_stamp.tm_hour);
}
static unsigned evalMinute(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%02d", log_ctx->time_stamp.tm_min);
}
static unsigned evalSecond(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%02d", log_ctx->time_stamp.tm_sec);
}
static unsigned evalDatetime(LogCtx* log_ctx, char* out, unsigned out_len) {
  return snprintf(out, out_len, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                  log_ctx->time_stamp.tm_year + 1900, log_ctx->time_stamp.tm_mon + 1,
                  log_ctx->time_stamp.tm_mday, log_ctx->time_stamp.tm_hour,
                  log_ctx->time_stamp.tm_min, log_ctx->time_stamp.tm_sec, log_ctx->ms);
}
static unsigned evalMsg(LogCtx* log_ctx, char* out, unsigned out_len) {
  return vsnprintf(out, out_len, log_ctx->msg_fmt, log_ctx->msg_args);
}
static unsigned evalVar(int var_id, LogCtx* log_ctx, char* out, unsigned out_len) {
  static unsigned (*eval_handlers[])(LogCtx*, char*, unsigned) = {
      evalPath,  evalLevel, evalFunc, evalFilename, evalTag,    evalLine,     evalYear,
      evalMonth, evalDay,   evalHour, evalMinute,   evalSecond, evalDatetime, evalMsg};
  return eval_handlers[var_id](log_ctx, out, out_len);
}

static unsigned Log_copy(char* out, unsigned out_len, const char* p, const char* q) {
  unsigned source_len = q ? q - p : strlen(p);
  int copy_len = out_len >= source_len + 1 ? source_len : out_len - 1;
  if (copy_len > 0) {
    memcpy(out, p, copy_len);
    out[copy_len] = 0;
  }
  return source_len;
}

static int Log_matchVarName(const char* var_name) {
  const char** s = &log_var_names[0];
  for (; s[0]; ++s) {
    if (strcmp(s[0], var_name) == 0)
      return s - log_var_names;
  }
  return -1;
}

typedef struct {
  const char* next;
  unsigned fmt_len;
} FormatPair;

static inline FormatPair FormatPair_make(const char* next, unsigned len) {
  FormatPair o = {.next = next, .fmt_len = len};
  return o;
}

static FormatPair Log_formatStep(const char* fmt, LogCtx* log_ctx, char* out, unsigned out_len) {
  const char* r;
  if (strncmp(fmt, "${", 2)) {
    r = strstr(fmt, "${");
    return FormatPair_make(r, Log_copy(out, out_len, fmt, r));
  }
  r = strchr(fmt + 2, '}');
  if (!r)
    return FormatPair_make(NULL, Log_copy(out, out_len, fmt, NULL));
  ++r;
  if (r - fmt - 3 > MAX_LOG_VAR)
    return FormatPair_make(r, Log_copy(out, out_len, fmt, r));
  char var_name[MAX_LOG_VAR + 1] = {0};
  memcpy(var_name, fmt + 2, r - fmt - 3);
  int var_id = Log_matchVarName(var_name);
  if (var_id >= 0)
    return FormatPair_make(r, evalVar(var_id, log_ctx, out, out_len));
  return FormatPair_make(r, Log_copy(out, out_len, fmt, r));
}

unsigned Log_format(const char* fmt, LogCtx* log_ctx, char* out, unsigned out_len) {
  unsigned off = 0;
  while (fmt && off < out_len) {
    FormatPair o = Log_formatStep(fmt, log_ctx, &out[off], out_len - off);
    fmt = o.next;
    off += o.fmt_len;
  }
  return off;
}
