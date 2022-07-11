//
// Created by xy on 7/10/22.
//

#include "xcl/lang/CBaseThreadImpl.h"
#include "xcl/lang/system.h"
#include "xcl/util/CBlocker.h"
#include "xcl/concurrent/CMutex.h"
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

void
__Thread_beforeCreate(CThread* thread) {
  __Thread_setAttach(thread, Blocker_new2(__Thread_mutex(thread)));
}
void
__Thread_afterCreate(CThread* thread) {}
void
__Thread_wait(CThread* thread) {
  pthread_join(__Thread_handle(thread), NULL);
}
bool
__Thread_waitTimeout(CThread* thread, int32_t timeout) {
  int64_t nanoTimeout = timeout * 1000000L;
  int64_t totalWait = 0;
  ThreadHandle handle = __Thread_handle(thread);
  int64_t st = nanos();
  while (totalWait < nanoTimeout) {
    int ret = pthread_kill(handle, 0);
    if (ret == ESRCH) {
      return true;
    }
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts = {0, nanoTimeout - totalWait};
    nanosleep(&ts, NULL);
#else
    usleep((nanoTimeout - totalWait) / 1000);
#endif
    totalWait = nanos() - st;
  }
  return pthread_kill(handle, 0) == ESRCH;
}
bool
__Thread_create(bool suspend,
                __ThreadRunProc run,
                void* usr,
                ThreadHandle* handle) {
  pthread_t h;
  int ret = pthread_create(&h, NULL, run, usr);
  if (ret == 0) {
    *handle = h;
    return true;
  }
  return false;
}
void
__Thread_resume(CThread* thread) {
  __Thread_setState(thread, ALIVE);
  Blocker_cancel((CBlocker*)__Thread_attach(thread));
}
void
__Thread_onStart(CThread* thread) {
  Blocker_wait((CBlocker*)__Thread_attach(thread));
}
void
__Thread_onFinish(CThread* thread, __ThreadRunReturnType retVal) {}
unsigned
__Thread_currentId() {
  return syscall(__NR_gettid);
}
ThreadHandle
__Thread_currentHandle(CThread* thread, unsigned tid) {
  return __Thread_handle(thread);
}
void
__Thread_finalize(CThread* thread) {
  Blocker_delete((CBlocker*)__Thread_attach(thread));
}
void
__Thread_detach(CThread* thread) {
  pthread_detach(__Thread_handle(thread));
}

#ifdef DYNAMIC
pthread_key_t __localStorageKey = 0;

static __attribute__((constructor)) void
__initKey() {
  pthread_key_create(&__localStorageKey, NULL);
}

CLocalStorage*
__Thread_getLocalStorage() {
  return pthread_getspecific(__localStorageKey);
}

bool
__Thread_setLocalStorage(CLocalStorage* localStorage) {
  int ret = pthread_setspecific(__localStorageKey, localStorage);
  return ret == 0;
}

#endif
