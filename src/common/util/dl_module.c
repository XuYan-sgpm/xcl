#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <xcl/io/path.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/util/comparators.h>
#include <xcl/util/dl_module.h>
#include <xcl/util/hash_map.h>
#include <xcl/util/log.h>
#include <xcl/util/string.h>
#include <xcl/util/vector.h>

static Vector* ex_load_list = NULL;

static void DlModule_freeExLoadList() {
  if (!ex_load_list)
    return;
  for (int i = 0; i < Vec_size(ex_load_list); i++) {
    String* s = Vec_pos(ex_load_list, i);
    String_destroy(s);
  }
  Vec_free(ex_load_list);
}

static Vector* DlModule_exLoadList() {
  if (!ex_load_list) {
    ex_load_list = Vec_new(sizeof(String));
    LOG_ASSERT(ex_load_list);
    postExit(DlModule_freeExLoadList);
  }
  return ex_load_list;
}

#ifdef Windows
#define SHARED_EXT "dll"
#define PATH_SEP '\\'

static bool DlModule_verifyDlExt(const char* ext) { return strcmp(ext, SHARED_EXT) == 0; }
#else
#define PATH_SEP '/'
#ifdef __APPLE__
#define SHARED_EXT "dylib"
#else
#define SHARED_EXT "so"
#endif
#include <ctype.h>

static bool DlModule_verifyDlExt(const char* ext) {
  if (strcmp(ext, SHARED_EXT) == 0)
    return true;
  const char* ver = strchr(ext, '.');
  if (!ver)
    return false;
  char real_ext[8] = {0};
  memcpy(real_ext, ext, ver - ext);
  if (strcmp(real_ext, SHARED_EXT))
    return false;
  const char* p = ver + 1;
  while (p[0] == '.' || isdigit(p[0]))
    ++p;
  if (p[0])
    return false;
  LOG_P(LOG_DEBUG, "library version %s", ver);
  return true;
}
#endif

#if defined(iOS)
static const char* DlModule_loadName(const char* name) {
  static char load_name[MAX_PATH_NAME];
  snprintf(load_name, sizeof(load_name), "%s.framework/%s", name);
  return load_name;
}
static Vector* DlModule_initExLoad() {
  Vector* vec = DlModule_exLoadList();
  if (Vec_empty(vec)) {
    String s = String_from(Path_dirname(Path_dirname(Path_module(DlModule_initExLoad))), -1);
    LOG_ASSERT(!String_empty(&s));
    LOG_ASSERT(Vec_push(vec, &s));
  }
  return vec;
}
#else
static Vector* DlModule_initExLoad() {
  Vector* vec = DlModule_exLoadList();
  if (Vec_empty(vec)) {
    String s = String_from(Path_dirname(Path_module(NULL)), -1);
    LOG_ASSERT(!String_empty(&s));
    LOG_ASSERT(Vec_push(vec, &s));
    s = String_from(Path_dirname(Path_module(DlModule_initExLoad)), -1);
    LOG_ASSERT(!String_empty(&s));
    LOG_ASSERT(Vec_push(vec, &s));
  }
  return vec;
}
static const char* DlModule_loadName(const char* name) { return name; }
#endif

static const char* DlModule_join(const char* hint, const char* name, char* path,
                                 unsigned path_len) {
  String dl_base = String_make();
  const char* dl_path = NULL;
  do {
    const char* ext = strchr(name, '.');
    if (ext && !DlModule_verifyDlExt(ext))
      break;
    dl_base = String_from(name, -1);
    if (!ext) {
      String_push(&dl_base, '.');
      String_append(&dl_base, SHARED_EXT, -1);
    }
    if (!(dl_path = Path_join(path, path_len, hint, String_str(&dl_base), NULL)))
      break;
    if (Path_exists(path))
      break;
    dl_path = NULL;
    if (strncmp(name, "lib", 3) == 0)
      break;
    String_insert(&dl_base, 0, "lib", -1);
    if ((dl_path = Path_join(path, path_len, hint, String_str(&dl_base), NULL)) &&
        Path_exists(path))
      break;
    dl_path = NULL;
  } while (false);
  String_destroy(&dl_base);
  return dl_path;
}

typedef struct {
  void* dl;
  void* sym_map;
  String dl_name;
} DlModule;

void* DlModule_new() {
  DlModule* dl_module = palloc(sizeof(DlModule));
  if (dl_module) {
    dl_module->dl_name = String_make();
    dl_module->dl = NULL;
    dl_module->sym_map = NULL;
    return dl_module;
  }
  pfree(dl_module);
  return NULL;
}

void DlModule_free(void* obj) {
  if (obj) {
    DlModule* dl_module = obj;
    Dl_close(dl_module->dl);
    HashMap_free(dl_module->sym_map);
    String_destroy(&dl_module->dl_name);
    pfree(obj);
  }
}

static bool DlModule_tryLoad(DlModule* dl_module, const char* hint) {
  char path[PATH_LIMIT];
  const char* name = String_str(&dl_module->dl_name);
  return DlModule_join(hint, String_str(&dl_module->dl_name), path, PATH_LIMIT) &&
         (dl_module->dl = Dl_open(path));
}

static bool DlModule_exLoad(DlModule* dl_module) {
  Vector* load_list = DlModule_initExLoad();
  if (!load_list)
    return false;
  unsigned i = 0;
  while (i < Vec_size(load_list) && !DlModule_tryLoad(dl_module, String_str(Vec_pos(load_list, i))))
    ++i;
  return dl_module->dl;
}

bool DlModule_open(void* obj, const char* name, ...) {
  DlModule* dl_module = obj;
  const char* load_name = DlModule_loadName(name);
  if (dl_module->dl) {
    if (!strcmp(String_str(&dl_module->dl_name), load_name))
      return true;
    LOG_P(LOG_ERROR, "dlmodule already opened: %s", String_str(&dl_module->dl_name));
    return false;
  }
  String_fill(&dl_module->dl_name, load_name, -1);
  if (String_empty(&dl_module->dl_name))
    return false;
  va_list ap;
  va_start(ap, name);
  const char* hint = va_arg(ap, const char*);
  while (hint && !DlModule_tryLoad(dl_module, hint))
    hint = va_arg(ap, const char*);
  va_end(ap);
  return dl_module->dl || DlModule_exLoad(dl_module) ||
         (dl_module->dl = Dl_open(String_str(&dl_module->dl_name)));
}

static bool DlModule_checkSymbolMap(DlModule* dl_module) {
  return dl_module->sym_map ||
         (dl_module->sym_map = HashMap_new(MAX_SYMBOL_NAME, sizeof(void*), NULL, scomp()));
}

void* DlModule_symbol(void* obj, const char* symbol) {
  DlModule* dl_module = obj;
  if (!dl_module->dl || strlen(symbol) >= MAX_SYMBOL_NAME || !DlModule_checkSymbolMap(dl_module))
    return NULL;
  char key[MAX_SYMBOL_NAME] = {0};
  strcpy(key, symbol);
  HashIter it = HashMap_find(dl_module->sym_map, key);
  if (HashIter_valid(it))
    return *(void**)HashIter_value(it);
  void* addr;
  if (!(addr = Dl_sym(dl_module->dl, symbol)))
    return NULL;
  it = HashMap_add(dl_module->sym_map, key, &addr);
  if (!HashIter_valid(it))
    LOG_P(LOG_WARNING, "cache symbol addr failed");
  return addr;
}

void DlModule_close(void* obj) {
  if (obj) {
    DlModule* dl_module = obj;
    if (dl_module->dl)
      Dl_close(dl_module->dl);
    dl_module->dl = NULL;
    if (dl_module->sym_map)
      HashMap_clear(dl_module->sym_map);
    String_clear(&dl_module->dl_name);
  }
}

const char* DlModule_name(void* obj) { return String_str(&((DlModule*)obj)->dl_name); }

bool DlModule_isOpened(void* obj, const char* name) {
  DlModule* dl_module = obj;
  return dl_module->dl && strcmp(String_str(&dl_module->dl_name), DlModule_loadName(name)) == 0;
}
