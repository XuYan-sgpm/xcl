#include <xcl/io/path.h>
#ifdef HAS_SENDFILE
#include <sys/sendfile.h>
#else
#include <sys/mman.h>
#endif
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <xcl/util/log.h>

char* Path_locals();
bool Path_isSeparator(char ch) { return ch == '/'; }

struct PathIter {
  DIR* ctx;
  char cwd[PATH_LIMIT];
};

PathIter* Path_iter(const char* dir) {
  PathIter* path_iter = malloc(sizeof(PathIter));
  if (path_iter && (path_iter->ctx = opendir(dir))) {
    strcpy(path_iter->cwd, dir);
    return path_iter;
  }
  free(path_iter);
  return NULL;
}
void PathIter_close(PathIter* path_iter) {
  if (path_iter) {
    closedir(path_iter->ctx);
    free(path_iter);
  }
}
const char* PathIter_next(PathIter* path_iter) {
  struct dirent* ent;
  while ((ent = readdir(path_iter->ctx))) {
    if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
      return ent->d_name;
  }
  return NULL;
}
const char* PathIter_dir(PathIter* path_iter) { return path_iter->cwd; }
char Path_separator() { return '/'; }
bool Path_mkdir(bool error_exists, const char* dir, ...) {
  va_list ap;
  va_start(ap, dir);
  int mode = va_arg(ap, int);
  va_end(ap);
  mode = mode <= 0 ? 0755 : mode & 0777;
  if (Path_exists(dir)) {
    if (error_exists) {
      LOG_P(LOG_ERROR, "directory %s already exists", dir);
      return false;
    }
  } else if (mkdir(dir, mode)) {
    LOG_P(LOG_ERROR, "mkdir %s failed:%d,%o", dir, errno, mode);
    return false;
  }
  return true;
}
static bool Path_doMkdirs(bool error_exists, char* dir, int mode) {
  if (access(dir, 0) == 0)
    return !error_exists;
  char* sep;
  if ((sep = strrchr(dir, '/'))) {
    sep[0] = 0;
    if (!Path_doMkdirs(error_exists, dir, mode))
      return false;
    sep[0] = '/';
  }
  return mkdir(dir, mode) == 0;
}
bool Path_mkdirs(bool error_exists, const char* dir, ...) {
  unsigned dir_len = strlen(dir);
  if (dir_len >= PATH_LIMIT)
    return false;
  va_list ap;
  va_start(ap, dir);
  int mode = va_arg(ap, int);
  va_end(ap);
  mode = mode <= 0 ? 0755 : mode & 0777;
  char buf[PATH_LIMIT];
  strcpy(buf, dir);
  if (buf[dir_len - 1] == '/')
    buf[dir_len - 1] = 0;
  return Path_doMkdirs(error_exists, buf, mode);
}
#ifdef __APPLE__
#include <mach-o/dyld.h>
const char* Path_exe() {
  uint32_t path_len = PATH_LIMIT;
  char* dest = Path_locals();
  if (_NSGetExecutablePath(dest, &path_len)) {
    LOG_P(LOG_ERROR, "_NSGetExecutablePath failed,buffer too small");
    return NULL;
  }
  return dest;
}
#else
const char* Path_exe() {
  char* dest = Path_locals();
  int ret = readlink("/proc/self/exe", dest, PATH_LIMIT);
  if (ret <= 0) {
    LOG_P(LOG_ERROR, "readlink /proc/self/exe failed:%d", errno);
    return NULL;
  }
  dest[ret] = 0;
  return dest;
}
#endif
const char* Path_module(void* addr) {
  if (!addr)
    return Path_exe();
  Dl_info info = {0};
  if (!dladdr(addr, &info)) {
    LOG_P(LOG_ERROR, "can not find module contains %p,%s", addr, dlerror());
    return NULL;
  }
  return realpath(info.dli_fname, Path_locals());
}
bool Path_exists(const char* path) { return path ? access(path, 0) == 0 : false; }
static bool Path_copyAttr(int fd1, int fd2) {
  struct stat info;
  if (fstat(fd1, &info)) {
    LOG_P(LOG_ERROR, "fstat error:%d", errno);
    return false;
  }
  if (fchmod(fd2, info.st_mode & 0777)) {
    LOG_P(LOG_ERROR, "fchmod %d failed:%d", fd2, errno);
    return false;
  }
  if (fchown(fd2, info.st_uid, info.st_gid)) {
    LOG_P(LOG_ERROR, "fchown %d [%d,%d] failed:%d", fd2, info.st_uid, info.st_gid, errno);
    return false;
  }
  return true;
}
static bool Path_fcopy(int rfd, int wfd) {
  off_t size = lseek(rfd, 0, SEEK_END);
  if (size < 0) {
    LOG_P(LOG_ERROR, "lseek failed:%d", errno);
    return false;
  }
  if (size == 0)
    return true;
  lseek(rfd, 0, SEEK_SET);
#ifdef HAS_SENDFILE
  off_t off = 0;
  ssize_t ret = sendfile(wfd, rfd, &off, size);
  return ret == size;
#else
  void* source = mmap(NULL, size, PROT_READ, MAP_PRIVATE, rfd, 0);
  if (source == MAP_FAILED) {
    LOG_P(LOG_ERROR, "map source file failed:%d", errno);
    return false;
  }
  for (off_t off = 0; off < size;) {
    ssize_t ret = write(wfd, source + off, size - off);
    if (ret < 0)
      return false;
    off += ret;
  }
  munmap(source, size);
  return true;
#endif
}
bool Path_copyReg(const char* src, const char* dest, int overwrite, ...) {
  va_list ap;
  va_start(ap, overwrite);
  int copy_attr = va_arg(ap, int);
  va_end(ap);
  bool complete = false;
  int fd1 = -1, fd2 = -1;
  do {
    bool dest_exists = Path_exists(dest);
    if (dest_exists && copy_attr) {
      LOG_P(LOG_ERROR, "file %s already exists,can not copy attributes", dest);
      break;
    }
    if (!overwrite && dest_exists) {
      LOG_P(LOG_ERROR, "file %s already exists", dest);
      break;
    }
    fd1 = open(src, O_RDONLY);
    if (fd1 < 0) {
      LOG_P(LOG_ERROR, "open %s failed:%d", src, errno);
      break;
    }
    if (dest_exists)
      fd2 = open(dest, O_WRONLY);
    else
      fd2 = open(dest, O_WRONLY | O_CREAT, 0755);
    if (fd2 < 0) {
      LOG_P(LOG_ERROR, "open %s failed:%d", dest, errno);
      break;
    }
    if (!Path_fcopy(fd1, fd2)) {
      LOG_P(LOG_ERROR, "copy %s -> %s failed", src, dest);
      break;
    }
    if (copy_attr && !Path_copyAttr(fd1, fd2)) {
      break;
    }
    complete = true;
  } while (false);
  if (fd1 >= 0)
    close(fd1);
  if (fd2 >= 0)
    close(fd2);
  return complete;
}
bool Path_remove0(const char* path, bool is_dir) {
  if (remove(path)) {
    LOG_P(LOG_ERROR, "remove %s failed:%d", path, errno);
    return false;
  }
  return true;
}
