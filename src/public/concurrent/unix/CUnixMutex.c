//
// Created by xy on 7/10/22.
//

#include <xcl/concurrent/CMutex.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "xcl/lang/system.h"
#include <stdio.h>
#include <unistd.h>

typedef struct {
  pthread_mutex_t handle;
} CUnixMutex;

XCL_PUBLIC void* XCL_API
Mutex_new() {
  CUnixMutex* mutex = malloc(sizeof(CUnixMutex));
  if (mutex) {
    memset(mutex, 0, sizeof(CUnixMutex));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(&mutex->handle, &attr) == 0) {
      return mutex;
    }
    free(mutex);
  }
  return NULL;
}

XCL_PUBLIC void XCL_API
Mutex_delete(void* mutex) {
  if (mutex) {
    pthread_mutex_destroy(&((CUnixMutex*)mutex)->handle);
    free(mutex);
  }
}

XCL_PUBLIC bool XCL_API
Mutex_lock(void* mutex) {
  if (mutex) {
    int ret = pthread_mutex_lock(&((CUnixMutex*)mutex)->handle);
    if (ret == 0) {
      return true;
    }
  }
  return false;
}

XCL_PUBLIC void XCL_API
Mutex_unlock(void* mutex) {
  if (mutex) {
    pthread_mutex_unlock(&((CUnixMutex*)mutex)->handle);
  }
}

XCL_PUBLIC bool XCL_API
Mutex_tryLock(void* mutex) {
  if (mutex) {
    return 0 == pthread_mutex_trylock(&((CUnixMutex*)mutex)->handle);
  }
  return false;
}

XCL_PUBLIC bool XCL_API
Mutex_tryLock2(void* mutex, int32_t millis) {
  if (mutex) {
#if !LINUX
    struct timespec ts = {0, 0};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += millis * 1000000;
    return 0 == pthread_mutex_timedlock(&((CUnixMutex*)mutex)->handle, &ts);
#else
    int64_t nanoTimeout = millis * 1000000L;
    int64_t totalWait = 0;
    int64_t st = nanos();
    int error;
    while (totalWait < nanoTimeout) {
      if ((error=pthread_mutex_trylock(&((CUnixMutex*)mutex)->handle)) == 0) {
        return true;
      }
      printf("err:%d\n",error);
#  if 0&&_POSIX_C_SOURCE >= 199309L
      struct timespec ts = {0, 1000000};
      nanosleep(&ts, NULL);
#  else
      usleep(1000);
#  endif
      totalWait = nanos() - st;
    }
    return pthread_mutex_trylock(&((CUnixMutex*)mutex)->handle) == 0;
#endif
  }
  return false;
}
