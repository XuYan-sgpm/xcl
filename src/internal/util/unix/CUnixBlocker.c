//
// Created by xy on 7/11/22.
//

#include <xcl/util/CBlocker.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

struct _CBlocker_st {
  pthread_mutex_t* mutex;
  pthread_cond_t cond;
  int32_t wait;
  bool externalLock;
};

CBlocker* XCL_API
Blocker_new() {
  CBlocker* blocker = malloc(sizeof(CBlocker));
  if (blocker) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    blocker->mutex = malloc(sizeof(pthread_mutex_t));
    if (blocker->mutex) {
      if (pthread_mutex_init(blocker->mutex, &attr) == 0) {
        if (pthread_cond_init(&blocker->cond, NULL) == 0) {
          blocker->wait = 0;
          blocker->externalLock = false;
          return blocker;
        }
        pthread_mutex_destroy(blocker->mutex);
      }
      free(blocker->mutex);
    }
    free(blocker);
  }
  return NULL;
}

CBlocker* XCL_API
Blocker_new2(void* mutex) {
  CBlocker* blocker = malloc(sizeof(CBlocker));
  if (blocker) {
    blocker->mutex = (pthread_mutex_t*)mutex;
    if (pthread_cond_init(&blocker->cond, NULL) == 0) {
      blocker->wait = 0;
      blocker->externalLock = true;
      return blocker;
    }
    free(blocker);
  }
  return NULL;
}

bool XCL_API
Blocker_delete(CBlocker* blocker) {
  int ret = pthread_mutex_lock(blocker->mutex);
  if (ret != 0) {
    return false;
  }
  bool releaseLock = false;
  bool allowRelease = false;
  ++blocker->wait;
  if (blocker->wait > 1) {
    // blocker is waiting
  } else if (blocker->wait < 1) {
    /**
     * some threads call notify or notifyAll
     * when blocker is not in waiting state
     */
    blocker->wait = 1;
  }
  --blocker->wait;
  if (blocker->wait == 0) {
    allowRelease = true;
    pthread_cond_destroy(&blocker->cond);
    releaseLock = !blocker->externalLock;
  }
  pthread_mutex_unlock(blocker->mutex);
  if (releaseLock) {
    pthread_mutex_destroy(blocker->mutex);
    free(blocker->mutex);
  }
  if (allowRelease) {
    free(blocker);
  }
  return allowRelease;
}

bool XCL_API
Blocker_wait(CBlocker* blocker) {
  int ret = pthread_mutex_lock(blocker->mutex);
  if (ret != 0) {
    return false;
  }
  ++blocker->wait;
  if (blocker->wait > 0) {
    ret = pthread_cond_wait(&blocker->cond, blocker->mutex);
  }
  pthread_mutex_unlock(blocker->mutex);
  return ret == 0;
}

bool XCL_API
Blocker_cancel(CBlocker* blocker) {
  int ret = pthread_mutex_lock(blocker->mutex);
  if (ret != 0) {
    return false;
  }
  if (blocker->wait >= 0) {
    if (blocker->wait > 0) {
      ret = pthread_cond_signal(&blocker->cond);
    }
    if (ret == 0) {
      --blocker->wait;
    }
  }
  pthread_mutex_unlock(blocker->mutex);
  return ret == 0;
}

bool XCL_API
Blocker_wakeAll(CBlocker* blocker) {
  int ret = pthread_mutex_lock(blocker->mutex);
  if (ret != 0) {
    return false;
  }
  if (blocker->wait > 0) {
    ret = pthread_cond_broadcast(&blocker->cond);
    if (ret == 0) {
      blocker->wait = 0;
    }
  }
  pthread_mutex_unlock(blocker->mutex);
  return ret == 0;
}
