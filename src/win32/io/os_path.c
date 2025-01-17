#include <Windows.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <util/log.h>
#include <xcl/util/string.h>
// #include <malloc.h>

char* Path_locals();
bool Path_isSeparator(char ch) { return ch == '/' || ch == '\\'; }
bool Path_exists(const char* path) { return path ? _access(path, 0) == 0 : false; }
bool Path_copyReg(const char* source, const char* dest, int overwrite, ...) {
  bool ret = CopyFile(source, dest, overwrite);
  if (!ret)
    LOG_P(LOG_DEBUG, "copy regular file %s -> %s failed:%u", source, dest, GetLastError());
  return ret;
}

struct PathIter {
  struct _finddata_t find_data;
  intptr_t find_handle;
  String list_dir;
};
static bool PathIter_assignDir(PathIter* path_iter, const char* dir) {
  path_iter->list_dir = String_from(dir, -1);
  if (String_size(&path_iter->list_dir)) {
    char ch = *String_back(&path_iter->list_dir);
    if (ch == '/' || ch == '\\')
      String_pop(&path_iter->list_dir);
    return true;
  }
  return false;
}
PathIter* Path_iter(const char* dir) {
  PathIter* path_iter = malloc(sizeof(PathIter));
  if (path_iter) {
    memset(path_iter, 0, sizeof(*path_iter));
    if (PathIter_assignDir(path_iter, dir))
      return path_iter;
    free(path_iter);
  }
  return NULL;
}
void PathIter_close(PathIter* path_iter) {
  if (path_iter) {
    if (path_iter->find_handle != -1)
      _findclose(path_iter->find_handle);
    String_destroy(&path_iter->list_dir);
    free(path_iter);
  }
}
static bool PathIter_doNext(PathIter* path_iter) {
  struct _finddata_t* find_data = &path_iter->find_data;
  if (!find_data->name[0]) {
    int old_size = String_size(&path_iter->list_dir);
    if (!String_append(&path_iter->list_dir, "\\*.*", -1))
      return false;
    path_iter->find_handle = _findfirst(String_str(&path_iter->list_dir), &path_iter->find_data);
    String_erase(&path_iter->list_dir, old_size, 4);
    if (path_iter->find_handle == -1) {
      LOG_P(LOG_ERROR, "_findfirst %s failed:%d", String_str(&path_iter->list_dir), errno);
      return false;
    }
  } else if (_findnext(path_iter->find_handle, find_data)) {
    if (errno != ENOENT)
      LOG_P(LOG_ERROR, "_findnext error:%d", errno);
    return false;
  }
  return true;
}
const char* PathIter_next(PathIter* path_iter) {
  while (PathIter_doNext(path_iter)) {
    if (strcmp(path_iter->find_data.name, ".") && strcmp(path_iter->find_data.name, ".."))
      return path_iter->find_data.name;
  }
  return NULL;
}
const char* PathIter_dir(PathIter* path_iter) { return String_str(&path_iter->list_dir); }
char Path_separator() { return '\\'; }
const char* Path_module(void* addr) {
  HMODULE handle;
  if (addr == NULL) {
    handle = NULL;
  } else if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                addr, &handle)) {
    LOG_P(LOG_ERROR, "get dl module handle failed:%u", GetLastError());
    return NULL;
  }
  char* dest = Path_locals();
  if (!GetModuleFileName(handle, dest, PATH_LIMIT)) {
    LOG_P(LOG_ERROR, "GetModuleFileName failed:%u", GetLastError());
    return NULL;
  }
  return dest;
}
static bool Path_recursiveMkDir(bool error_exists, char* dir) {
  if (Path_exists(dir))
    return !error_exists;
  char* sep;
  if ((sep = strrchr(dir, '/')) || (sep = strrchr(dir, '\\'))) {
    sep[0] = 0;
    if (!Path_recursiveMkDir(error_exists, dir))
      return false;
    sep[0] = '\\';
  }
  if (_mkdir(dir)) {
    LOG_P(LOG_ERROR, "_mkdir %s error:%d", dir, errno);
    return false;
  }
  return true;
}
bool Path_mkdir(bool error_exists, const char* dir, ...) {
  if (Path_exists(dir)) {
    if (error_exists) {
      LOG_P(LOG_ERROR, "directory %s already exists", dir);
      return false;
    }
  } else if (_mkdir(dir)) {
    LOG_P(LOG_ERROR, "_mkdir %s error:%d", dir, errno);
    return false;
  }
  return true;
}
bool Path_mkdirs(bool error_exists, const char* dir, ...) {
  unsigned len = strlen(dir);
  if (len >= PATH_LIMIT)
    return false;
  char tmp[PATH_LIMIT];
  strcpy(tmp, dir);
  if (dir[len] == '/' || dir[len] == '\\')
    --len;
  tmp[len] = 0;
  return Path_recursiveMkDir(error_exists, tmp);
}
