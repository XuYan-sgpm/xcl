//
// Created by xy on 7/10/22.
//

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/lang/system.h>

void* Mutex_new() {
  void* mut = malloc(sizeof(pthread_mutex_t));
  if (!mut)
    return NULL;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  int err = pthread_mutex_init(mut, &attr);
  pthread_mutexattr_destroy(&attr);
  if (!err)
    return mut;
  free(mut);
  return NULL;
}

bool Mutex_delete(void* mutex) {
  if (!mutex)
    return false;
  bool ret = pthread_mutex_destroy(mutex) == 0;
  free(mutex);
  return ret;
}

bool Mutex_lock(void* mutex) { return mutex && pthread_mutex_lock(mutex) == 0; }

bool Mutex_unlock(void* mutex) { return mutex && pthread_mutex_unlock(mutex) == 0; }

bool Mutex_tryLock(void* mutex) { return mutex && pthread_mutex_trylock(mutex) == 0; }

#if defined(Linux) && _POSIX_C_SOURCE >= 199309L
static int Mutex_tryLockFor(void* mutex, int millis) {
  struct timespec ts = {0, 0};
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += millis / 1000;
  millis %= 1000;
  ts.tv_nsec += (int64_t)millis * 1000000L;
  ts.tv_sec += ts.tv_nsec / 1000000000;
  ts.tv_nsec %= 1000000000;
  return pthread_mutex_timedlock(mutex, &ts);
}
#else
static int Mutex_tryLockFor(void* mutex, int millis) {
  int err;
  unsigned wait_limit = millis < 0 ? 0xffffffff : millis;
  int64_t deadline = nanos() + wait_limit * 1000000;
  int64_t now;
  while ((err = pthread_mutex_trylock(mutex)) && (now = nanos()) < deadline)
    usleep(1000);
  return err;
}
#endif
bool Mutex_timedLock(void* mutex, int32_t millis) {
  if (!mutex)
    return false;
  int err = Mutex_tryLockFor(mutex, millis);
  if (err)
    errno = err;
  return !err;
}
