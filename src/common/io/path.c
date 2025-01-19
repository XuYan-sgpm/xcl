#include <errno.h>
#include <internal/lang/local_reserve_id.h>
#include <internal/lang/local_storage.h>
#include <internal/xcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <xcl/io/path.h>
#include <xcl/lang/thread_local.h>
#include <xcl/util/log.h>
#include <xcl/util/page.h>
#include <xcl/util/string.h>

#ifdef Windows
#include <direct.h>
#define stat _stat
#define rmdir _rmdir
#ifndef _S_ISTYPE
#define _S_ISTYPE(mode, mask) (((mode) & _S_IFMT) == (mask))
#define S_ISREG(mode) _S_ISTYPE((mode), _S_IFREG)
#define S_ISDIR(mode) _S_ISTYPE((mode), _S_IFDIR)
#endif
static const char path_seps[] = {'\\', '/'};
#else
#include <unistd.h>
static const char path_seps[] = {'/'};
#endif

static bool Path_isSeparator(char ch) {
  return ch == path_seps[0] || (sizeof(path_seps) > 1 && ch == path_seps[1]);
}

char* Path_locals();
bool Path_copyReg(const char* source, const char* dest, int overwrite, ...);
bool Local_set0(uintptr_t id, LocalPair pair);
LocalPair* Local_get0(uintptr_t id);

const char* Path_basename(const char* path) {
  return Path_basename_r(path, Path_locals(), PATH_LIMIT);
}
const char* Path_basename_r(const char* path, char* out, unsigned out_len) {
  char* dest = out;
  const char* end = path + strlen(path);
  const char* base = end - 1;
  while (base >= path && !Path_isSeparator(base[0]))
    --base;
  ++base;
  if (out_len <= end - base)
    return NULL;
  memmove(dest, base, end - base);
  dest[end - base] = 0;
  return dest;
}
const char* Path_dirname(const char* path) {
  return Path_dirname_r(path, Path_locals(), PATH_LIMIT);
}
const char* Path_dirname_r(const char* path, char* out, unsigned out_len) {
  char* dest = out;
  const char* r = path + strlen(path) - 1;
  while (r >= path && !Path_isSeparator(r[0]))
    --r;
  if (r < path)
    ++r;
  if (r - path >= out_len)
    return NULL;
  memmove(dest, path, r - path);
  dest[r - path] = 0;
  return dest;
}
static void Path_strip(const char* path, const char** l, const char** r) {
  const char *p = path, *q = path + strlen(path);
  while (p < q && Path_isSeparator(p[0]))
    ++p;
  while (p < q && Path_isSeparator(q[-1]))
    --q;
  if (l)
    *l = p;
  if (r)
    *r = q;
}
bool Path_create(const char* path, bool is_dir) {
  if (Path_exists(path))
    return true;
  if (!is_dir) {
    FILE* fp = fopen(path, "w");
    fclose(fp);
    return fp;
  }
  return Path_mkdirs(false, path, 0);
}
static char* Path_append2buf(char* out, unsigned out_len, const char* name) {
  const char *l, *r;
  Path_strip(name, &l, &r);
  if (out_len <= r - l) {
    LOG_P(LOG_ERROR, "out buffer length %d too small, need %d", out_len, r - l + 1);
    return NULL;
  }
  memmove(out, l, r - l);
  out[r - l] = 0;
  return &out[r - l];
}
static char* Path_joinInit(char* out, unsigned out_len, const char* name) {
#ifndef Windows
  if (out_len <= 1)
    return NULL;
  *out++ = '/';
  --out_len;
#endif
  return Path_append2buf(out, out_len, name);
}
static char* Path_joinNext(char* out, unsigned out_len, const char* name) {
  if (out_len > 1) {
    *out++ = Path_separator();
    out = Path_append2buf(out, --out_len, name);
    return out;
  }
  return NULL;
}
char* Path_vjoin(char* out, unsigned out_len, va_list ap) {
  const char* end = out + out_len;
  const char* name = va_arg(ap, const char*);
  char* p = Path_joinInit(out, out_len, name);
  while (p && (name = va_arg(ap, const char*)))
    p = Path_joinNext(p, end - p, name);
  return p ? out : NULL;
}
char* Path_join(char* out, unsigned out_len, ...) {
  va_list ap;
  va_start(ap, out_len);
  char* ret = Path_vjoin(out, out_len, ap);
  va_end(ap);
  return ret;
}
static bool Path_append2s(const char* name, String* s) {
  const char *l, *r;
  Path_strip(name, &l, &r);
  return String_append(s, l, r - l);
}
static bool Path_joinInits(const char* name, String* s) {
#ifndef Windows
  String_push(s, '/');
#endif
  return Path_append2s(name, s);
}
static inline bool Path_joinNexts(const char* name, String* s) {
  return String_push(s, Path_separator()) && Path_append2s(name, s);
}
String Path_vjoins(const char* name, va_list ap) {
  String s = String_make();
  bool ret = Path_joinInits(name, &s);
  while (ret && (name = va_arg(ap, const char*)))
    ret = Path_joinNexts(name, &s);
  if (!ret)
    String_destroy(&s);
  return s;
}
String Path_joins(const char* name, ...) {
  va_list ap;
  va_start(ap, name);
  String s = Path_vjoins(name, ap);
  va_end(ap);
  return s;
}

typedef struct {
  PathHandler super;
} RemovePathHandler;

static bool RemovePathHandler_process(PathHandler* obj, const char* path) {
  struct stat info;
  if (stat(path, &info)) {
    LOG_P(LOG_ERROR, "stat %s error:%d", path, errno);
    return false;
  }
  if (S_ISREG(info.st_mode)) {
    if (remove(path)) {
      LOG_P(LOG_ERROR, "remove file %s error:%d", path, errno);
      return false;
    }
    return true;
  }
  if (!Path_listDir(path, obj))
    return false;
  if (rmdir(path) == 0)
    return true;
  LOG_P(LOG_ERROR, "remove directory %s error:%d", path, errno);
  return false;
}
static RemovePathHandler RemovePathHandler_make() {
  static void* rm_path_handler_vptr[] = {RemovePathHandler_process};
  RemovePathHandler rm_path_handler = {.super.__vptr = (void*)rm_path_handler_vptr};
  return rm_path_handler;
}

bool Path_remove(const char* path) {
  if (!Path_exists(path))
    return false;
  RemovePathHandler rm_path_handler = RemovePathHandler_make();
  return PathHandler_process(&rm_path_handler.super, path);
}

bool Path_listDir(const char* dir, PathHandler* path_handler) {
  if (!Path_exists(dir))
    return false;
  PathIter* iter = Path_iter(dir);
  if (!iter)
    return false;
  const char* name;
  while ((name = PathIter_next(iter))) {
    char path[PATH_LIMIT];
    if (!Path_join(path, PATH_LIMIT, PathIter_dir(iter), name, NULL))
      continue;
    if (!path_handler->__vptr->process(path_handler, path))
      break;
  }
  PathIter_close(iter);
  return !name;
}

typedef struct {
  PathHandler super;
  PathHandler* underlying_handler;
} WalkPathHandler;

static bool WalkPathHandler_process(PathHandler* obj, const char* path) {
  struct stat info;
  if (stat(path, &info)) {
    LOG_P(LOG_ERROR, "stat %s error:%d", path, errno);
    return false;
  }
  WalkPathHandler* wp_handler = (void*)obj;
  return (!S_ISDIR(info.st_mode) || Path_listDir(path, obj)) &&
         PathHandler_process(wp_handler->underlying_handler, path);
}
static WalkPathHandler WalkPathHandler_make(PathHandler* path_handler) {
  static void* walk_path_handler_vptr[] = {WalkPathHandler_process};
  WalkPathHandler wp_handler = {0};
  wp_handler.underlying_handler = path_handler;
  wp_handler.super.__vptr = (void*)walk_path_handler_vptr;
  return wp_handler;
}

bool Path_walk(const char* dir, PathHandler* path_handler) {
  if (!Path_exists(dir))
    return false;
  WalkPathHandler wp_handler = WalkPathHandler_make(path_handler);
  return PathHandler_process(&wp_handler.super, dir);
}

typedef struct {
  PathHandler super;
  String dest;
  bool overwrite;
  bool copy_attr;
} CopyPathHandler;

static bool CopyPathHandler_cpyWithStat(CopyPathHandler* cp_handler, const char* path,
                                        struct stat info) {
  return S_ISREG(info.st_mode) ? Path_copyReg(path, String_str(&cp_handler->dest),
                                              cp_handler->overwrite, cp_handler->copy_attr)
                               : Path_mkdir(false, String_str(&cp_handler->dest), info.st_mode) &&
                                     Path_listDir(path, &cp_handler->super);
}

static bool CopyPathHandler_process(PathHandler* obj, const char* path) {
  CopyPathHandler* cp_handler = (void*)obj;
  bool ret = false;
  do {
    struct stat info;
    if (stat(path, &info)) {
      LOG_P(LOG_ERROR, "stat %s error:%d", path, errno);
      break;
    }
    unsigned size = String_size(&cp_handler->dest);
    ret = String_push(&cp_handler->dest, Path_separator()) &&
          String_append(&cp_handler->dest, Path_basename(path), -1) &&
          CopyPathHandler_cpyWithStat(cp_handler, path, info);
    String_erase(&cp_handler->dest, size, -1);
  } while (false);
  return ret;
}
static CopyPathHandler CopyPathHandler_make(const char* dest, int copy_flag) {
  static void* copy_path_handler_vptr[] = {CopyPathHandler_process};
  CopyPathHandler cp_handler = {0};
  cp_handler.dest = String_from(dest, -1);
  cp_handler.overwrite = copy_flag & PATH_COPY_OVERWRITE;
  cp_handler.copy_attr = copy_flag & PATH_COPY_ATTR;
  cp_handler.super.__vptr = (void*)copy_path_handler_vptr;
  return cp_handler;
}

bool Path_copy(const char* source, const char* dest, int copy_flag) {
  if (!Path_exists(source) || !Path_exists(dest)) {
    LOG_P(LOG_DEBUG, "%s or %s is not exists", source, dest);
    return false;
  }
  CopyPathHandler cp_handler = CopyPathHandler_make(dest, copy_flag);
  bool ret = String_size(&cp_handler.dest) && PathHandler_process(&cp_handler.super, source);
  String_destroy(&cp_handler.dest);
  return ret;
}

typedef struct {
  PathHandler super;
} DebugPathHandler;

static bool DebugPathHandler_process(PathHandler* obj, const char* path) {
  LOG_P(LOG_DEBUG, "debug path:%s", path);
  return true;
}
PathHandler* Path_debugHandler() {
  static void* dbg_handler_vptr[] = {DebugPathHandler_process};
  static DebugPathHandler dbg_handler = {.super.__vptr = (void*)dbg_handler_vptr};
  return &dbg_handler.super;
}

char* Path_locals() {
  Xcl_init(XCL_LOCAL_KEY);
  char* buf = Local_get0(LOCAL_ID_PATH_BUFFER)->attach;
  if (!buf) {
    buf = malloc(PATH_LIMIT);
    LOG_ASSERT_PMSG(buf != NULL, "alloc path buffer failed");
    Local_set0(LOCAL_ID_PATH_BUFFER, LocalPair_make(buf, free));
  }
  return buf;
}
