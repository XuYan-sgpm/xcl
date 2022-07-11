//
// Created by xy on 7/10/22.
//

#include <xcl/concurrent/CCond.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  pthread_cond_t handle;
} CUnixCond;

XCL_PUBLIC void* XCL_API
Cond_new() {
  CUnixCond* cond = malloc(sizeof(CUnixCond));
  if (cond) {
    memset(cond, 0, sizeof(CUnixCond));
    int ret = pthread_cond_init(&cond->handle, NULL);
    if (ret == 0) {
      return cond;
    }
    free(cond);
  }
  return NULL;
}
XCL_PUBLIC void XCL_API
Cond_delete(void* cond) {
  if (cond) {
    pthread_cond_destroy(&((CUnixCond*)cond)->handle);
    free(cond);
  }
}
XCL_PUBLIC bool XCL_API
Cond_wait(void* mutex, void* cond) {
  if (mutex && cond) {
    int ret = pthread_cond_wait(cond, mutex);
    return ret == 0;
  }
  return false;
}
XCL_PUBLIC bool XCL_API
Cond_waitFor(void* mutex, void* cond, int32_t millis) {
  if (mutex && cond) {
    struct timespec ts = {0, millis * 1000000};
    return pthread_cond_timedwait(cond, mutex, &ts) == 0;
  }
  return false;
}
XCL_PUBLIC void XCL_API
Cond_signal(void* cond) {
  if (cond) {
    pthread_cond_signal(cond);
  }
}
XCL_PUBLIC void XCL_API
Cond_signalAll(void* cond) {
  if (cond) {
    pthread_cond_broadcast(cond);
  }
}
