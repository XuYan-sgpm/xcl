

#include <dlfcn.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <xcl/lang/system.h>
#include <xcl/util/log.h>

int64_t currentMillis() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int64_t s1 = (int64_t)(time.tv_sec) * 1000;
  int64_t s2 = (time.tv_usec / 1000);
  return s1 + s2;
}

int64_t nanos() {
#ifdef CLOCK_MONOTONIC
  struct timespec tv = {0, 0};
  clock_gettime(CLOCK_REALTIME, &tv);
  return (int64_t)tv.tv_sec * 1000000000 + tv.tv_nsec;
#else
  struct timeval time;
  gettimeofday(&time, NULL);
  int64_t s1 = (int64_t)(time.tv_sec) * 1000000;
  int64_t s2 = (time.tv_usec);
  return 1000 * (s1 + s2);
#endif
}

void msleep(int32_t timeout) {
#if _POSIX_C_SOURCE >= 199309L
  struct timespec ts = {0, 0};
  ts.tv_sec = timeout / 1000;
  ts.tv_nsec = (int64_t)(timeout - ts.tv_sec * 1000) * 1000000L;
  nanosleep(&ts, NULL);
#else
  usleep((int64_t)timeout * 1000);
#endif
}

unsigned error(int op, ...) {
  if (op == 1) {
    va_list ap;
    va_start(ap, op);
    errno = va_arg(ap, unsigned);
    va_end(ap);
  }
  return errno;
}

void* Dl_open(const char* str) { return dlopen(str, RTLD_NOW); }

void* Dl_sym(void* handle, const char* symbol) { return dlsym(handle, symbol); }

void Dl_close(void* handle) {
  if (handle)
    dlclose(handle);
}

int _gettimeofday(struct _timeval* tv, void* zone) {
  struct timeval tmp;
  int ret = gettimeofday(&tmp, NULL);
  if (ret == 0) {
    tv->tv_sec = tmp.tv_sec;
    tv->tv_usec = tmp.tv_usec;
  }
  return ret;
}
