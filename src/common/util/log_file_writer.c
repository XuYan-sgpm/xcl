#include <internal/util/log_file_writer.h>
#include <internal/util/log_format.h>
#include <stdio.h>
#include <string.h>
#include <xcl/io/path.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/util/log.h>

static bool LogFileWriter_slice(LogFileWriter* writer, char* out, unsigned out_len) {
  char postfix[32];
  const char* name = String_str(&writer->name);
  const char* ext = strrchr(name, '.');
  const char* p = ext - 1;
  const char* pure_name_ed;
  while (p >= name && p[0] != '.')
    --p;
  if (p < name) {
    pure_name_ed = ext;
    snprintf(postfix, sizeof(postfix), ".[slice1].log");
  } else {
    if (strncmp(p, ".[slice", 7))
      return false;
    pure_name_ed = p;
    p += 7;
    const char* q = strchr(p, ']');
    if (!q || strcmp(q + 1, ".log"))
      return false;
    int part_no = 0;
    while (p < q && p[0] >= '0' && p[0] <= '9')
      part_no = part_no * 10 + (*p++ - '0');
    if (p != q)
      return false;
    snprintf(postfix, sizeof(postfix), ".[slice%d].log", part_no + 1);
  }
  if (out_len <= pure_name_ed - name + strlen(postfix))
    return false;
  memcpy(out, name, pure_name_ed - name);
  strcpy(&out[pure_name_ed - name], postfix);
  LOG_P(LOG_DEBUG, "slice log %s -> %s", String_str(&writer->name), out);
  return true;
}

static char* LogFileWriter_makeName(LogFileWriter* writer, char* name, unsigned name_len,
                                    LogCtx* log_ctx) {
  unsigned ret = Log_format(String_str(&writer->name_fmt), log_ctx, name, name_len);
  LOG_ASSERT(ret > 0);
  LOG_P(LOG_DEBUG, "format log file name -> %s", name);
  return name;
}

static bool LogFileWriter_finish(LogFileWriter* writer) {
  LOG_ASSERT(!String_empty(&writer->name));
  fclose(writer->fp);
  writer->fp = NULL;
  if (!writer->manager)
    return true;
  char path[PATH_LIMIT];
  if (!Path_join(path, PATH_LIMIT, String_str(&writer->dir), String_str(&writer->name), NULL) ||
      !LogManager_add(writer->manager, path, writer->curr_fp_ctime)) {
    LOG_P(LOG_ERROR, "add log manager file failed");
    return false;
  }
  writer->curr_write_limit = LogManager_currWriteLimit(writer->manager);
  return true;
}

static bool LogFileWriter_updateTm(LogFileWriter* writer) {
  time_t now = time(NULL);
  struct tm o = *localtime(&now);
  if (o.tm_year == writer->last_check_point.tm_year &&
      o.tm_mon == writer->last_check_point.tm_mon && o.tm_mday == writer->last_check_point.tm_mday)
    return false;
  writer->last_check_point = o;
  return true;
}

static bool LogFileWriter_open(LogFileWriter* writer, const char* name) {
  char path[PATH_LIMIT];
  Path_join(path, sizeof(path), String_str(&writer->dir), name, NULL);
  writer->fp = fopen(path, "ab");
  if (!writer->fp) {
    LOG_P(LOG_ERROR, "open %s failed", path);
    return false;
  }
  struct _timeval tv = {0};
  _gettimeofday(&tv, NULL);
  writer->curr_fp_ctime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  String_fill(&writer->name, name, -1);
  return true;
}

static void LogFileWriter_reserve(LogFileWriter* writer) {
  if (!writer->manager) {
    LOG_ASSERT(writer->curr_write_limit < 0);
    return;
  }
  while (writer->curr_write_limit == 0 ||
         (writer->manager->config.max_logs >= 0 &&
          LogManager_window(writer->manager) == writer->manager->config.max_logs)) {
    LogManager_pop(writer->manager);
    writer->curr_write_limit = LogManager_currWriteLimit(writer->manager);
  }
}

static bool LogFileWriter_initialPreWrite(LogFileWriter* writer, LogCtx* log_ctx) {
  char name[MAX_PATH_NAME];
  LogFileWriter_makeName(writer, name, sizeof(name), log_ctx);
  LogFileWriter_reserve(writer);
  return LogFileWriter_open(writer, name);
}

static bool LogFileWriter_nextPreWrite(LogFileWriter* writer, LogCtx* log_ctx) {
  char name[MAX_PATH_NAME];
  if (!LogFileWriter_updateTm(writer) ||
      strcmp(String_str(&writer->name),
             LogFileWriter_makeName(writer, name, sizeof(name), log_ctx)) == 0) {
    if (writer->curr_write_limit != 0)
      return true;
    if (!LogFileWriter_slice(writer, name, MAX_PATH_NAME))
      return false;
  }
  if (!LogFileWriter_finish(writer))
    return false;
  LogFileWriter_reserve(writer);
  return LogFileWriter_open(writer, name);
}

static inline bool LogFileWriter_preWrite(LogFileWriter* writer, LogCtx* log_ctx) {
  bool (*f)(LogFileWriter*, LogCtx*) =
      String_empty(&writer->name) ? LogFileWriter_initialPreWrite : LogFileWriter_nextPreWrite;
  return f(writer, log_ctx);
}

bool LogFileWriter_init(LogFileWriter* writer, const char* log_file_fmt, LogManager* manager) {
  memset(writer, 0, sizeof(*writer));
  writer->name = writer->name_fmt = String_make();
  writer->dir = String_make();
  const char* name = Path_basename(log_file_fmt);
  const char* ext = strrchr(name, '.');
  if (!ext || strcmp(ext, ".log"))
    return false;
  writer->name_fmt = String_from(name, -1);
  writer->dir = String_from(Path_dirname(log_file_fmt), -1);
  LOG_P(LOG_DEBUG, "name: %s, log dir: %s", String_str(&writer->name_fmt),
        String_str(&writer->dir));
  if (manager && !LogManager_exec(manager, String_str(&writer->dir), false)) {
    String_destroy(&writer->name_fmt);
    String_destroy(&writer->dir);
    return false;
  }
  writer->curr_write_limit = !manager ? -1 : LogManager_currWriteLimit(manager);
  writer->manager = manager;
  time_t now = time(NULL);
  writer->last_check_point = *localtime(&now);
  return true;
}

void LogFileWriter_destroy(LogFileWriter* writer) {
  String_destroy(&writer->name);
  String_destroy(&writer->name_fmt);
  String_destroy(&writer->dir);
  if (writer->fp)
    fclose(writer->fp);
  writer->fp = NULL;
  writer->curr_write_limit = 0;
  writer->curr_fp_ctime = -1;
  writer->manager = NULL;
}

static int LogFileWriter_tryWrite(LogFileWriter* writer, const void* buf, unsigned len,
                                  LogCtx* log_ctx) {
  if (!LogFileWriter_preWrite(writer, log_ctx))
    return -1;
  int write_size = writer->curr_write_limit < 0 || writer->curr_write_limit >= len
                       ? len
                       : writer->curr_write_limit;
  if (write_size) {
    fwrite(buf, 1, write_size, writer->fp);
    fflush(writer->fp);
    writer->curr_write_limit -= write_size;
  }
  return write_size;
}
bool LogFileWriter_write(LogFileWriter* writer, const void* buf, unsigned len, LogCtx* log_ctx) {
  while (len > 0) {
    int write_size = LogFileWriter_tryWrite(writer, buf, len, log_ctx);
    if (write_size <= 0)
      return false;
    buf = (const char*)buf + write_size;
    len -= write_size;
  }
  return true;
}

void LogFileWriter_changeNameFmt(LogFileWriter* writer, const char* name_fmt) {
  LogFileWriter_finish(writer);
  String_fill(&writer->name_fmt, name_fmt, -1);
  String_clear(&writer->name);
}
