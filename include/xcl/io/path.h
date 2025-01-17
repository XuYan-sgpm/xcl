//
// Created by 14049 on 2024/5/29.
//

#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <xcl/lang/define.h>
#include <xcl/util/string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PATH_NAME 256
#define PATH_LIMIT 4096

typedef struct PathIter PathIter;
typedef struct PathHandler PathHandler;
struct PathHandler {
  struct {
    bool (*process)(PathHandler*, const char*);
  }* __vptr;
};
inline always_inline bool PathHandler_process(PathHandler* obj, const char* path) {
  return obj->__vptr->process(obj, path);
}

typedef enum { PATH_COPY_OVERWRITE = 1, PATH_COPY_ATTR = 2 } PathCopyFlag;

bool Path_exists(const char* path);
bool Path_remove(const char* path);
bool Path_copy(const char* source, const char* dest, int copy_flag);
bool Path_listDir(const char* dir, PathHandler* path_handler);
bool Path_walk(const char* dir, PathHandler* path_handler);
PathIter* Path_iter(const char* dir);
void PathIter_close(PathIter* path_iter);
const char* PathIter_next(PathIter* path_iter);
const char* PathIter_dir(PathIter* path_iter);
char Path_separator();
char* Path_vjoin(char* out, unsigned out_len, va_list ap);
char* Path_join(char* out, unsigned out_len, ...);
String Path_vjoins(const char* s, va_list ap);
String Path_joins(const char* s, ...);
bool Path_mkdir(bool error_exists, const char* dir, ...);
bool Path_mkdirs(bool error_exists, const char* dir, ...);
const char* Path_basename(const char* path);
const char* Path_basename_r(const char* path, char* out, unsigned out_len);
const char* Path_dirname(const char* path);
const char* Path_dirname_r(const char* path, char* out, unsigned out_len);
bool Path_create(const char* path, bool is_dir);
const char* Path_module(void* addr);
PathHandler* Path_debugHandler();

#ifdef __cplusplus
}
#endif
