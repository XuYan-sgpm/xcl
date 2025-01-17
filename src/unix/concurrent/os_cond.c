//
// Created by xy on 7/10/22.
//

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/concurrent/condition.h>

void* Cond_new() {
  void* cond = malloc(sizeof(pthread_cond_t));
  if (!cond)
    return NULL;
  int err = pthread_cond_init(cond, NULL);
  if (err == 0)
    return cond;
  errno = err;
  free(cond);
  return NULL;
}

bool Cond_delete(void* cond) {
  if (!cond)
    return false;
  int err = pthread_cond_destroy(cond);
  free(cond);
  if (err)
    errno = err;
  return !err;
}

bool Cond_wait(void* mutex, void* cond) {
  if (!mutex || !cond)
    return false;
  int err = pthread_cond_wait(cond, mutex);
  if (err)
    errno = err;
  return !err;
}

bool Cond_waitFor(void* mutex, void* cond, unsigned millis) {
  if (!mutex || !cond)
    return false;
  struct timespec ts = {0, 0};
  ts.tv_sec = millis / 1000;
  ts.tv_nsec = (millis % 1000) * 1000000;
  int err = pthread_cond_timedwait(cond, mutex, &ts);
  if (err)
    errno = err;
  return !err;
}

bool Cond_signal(void* cond) {
  if (!cond)
    return false;
  int err = pthread_cond_signal(cond);
  if (err)
    errno = err;
  return !err;
}

bool Cond_broadcast(void* cond) {
  if (!cond)
    return false;
  int err = pthread_cond_broadcast(cond);
  if (err)
    errno = err;
  return !err;
}
