//
// Created by xy on 7/10/22.
//

#include <xcl/concurrent/CCond.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
  pthread_cond_t handle;
} CUnixCond;

XCL_EXPORT(void*)
Cond_new() {
  CUnixCond* cond = malloc(sizeof(CUnixCond));
  if (cond) {
    memset(cond, 0, sizeof(CUnixCond));
    int ret = pthread_cond_init(&cond->handle, NULL);
    if (ret == 0) {
      return cond;
    }
    errno = ret;
    free(cond);
  }
  return NULL;
}
XCL_EXPORT(bool)
Cond_delete(void* cond) {
  if (cond) {
    int ret = pthread_cond_destroy(&((CUnixCond*)cond)->handle);
    free(cond);
    if (ret)
      errno = ret;
    return !ret;
  }
  return false;
}
XCL_EXPORT(bool)
Cond_wait(void* mutex, void* cond) {
  if (mutex && cond) {
    int ret = pthread_cond_wait(cond, mutex);
    if (ret)
      errno = ret;
    return !ret;
  }
  return false;
}
XCL_EXPORT(bool)
Cond_waitFor(void* mutex, void* cond, int32_t millis) {
  if (mutex && cond) {
    struct timespec ts = {0, millis * 1000000};
    int ret = pthread_cond_timedwait(cond, mutex, &ts);
    if (ret)
      errno = ret;
    return !ret;
  }
  return false;
}
XCL_EXPORT(bool)
Cond_signal(void* cond) {
  if (cond) {
    int ret = pthread_cond_signal(cond);
    if (ret)
      errno = ret;
    return !ret;
  }
  return false;
}
XCL_EXPORT(bool)
Cond_signalAll(void* cond) {
  if (cond) {
    int ret = pthread_cond_broadcast(cond);
    if (ret)
      errno = ret;
    return !ret;
  }
  return false;
}
