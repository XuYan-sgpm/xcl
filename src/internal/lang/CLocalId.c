//
// Created by xuyan on 2022/7/8.
//

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "xcl/concurrent/CMutex.h"

typedef struct {
  int32_t* freeIdList;
  void* freeIdLock;
  int32_t size;
  int32_t cap;
} __LocalIdQueue;

static __LocalIdQueue __idQueue;

void
__LocalId_initQueue() {
  memset(&__idQueue, 0, sizeof(__idQueue));
  __idQueue.cap = 8;
  int32_t* idList = malloc(__idQueue.cap * sizeof(int32_t));
  if (idList) {
    void* lock = Mutex_new();
    if (lock) {
      memset(idList, 0, sizeof(int32_t) * __idQueue.cap);
      __idQueue.freeIdLock = lock;
      __idQueue.freeIdList = idList;
      return;
    }
    free(idList);
  }
}

static void
__LocalId_releaseQueue() {
  if (__idQueue.freeIdList) {
    free(__idQueue.freeIdList);
    Mutex_delete(__idQueue.freeIdLock);
  }
  memset(&__idQueue, 0, sizeof(__idQueue));
}

static bool
__LocalId_offerQueue(int32_t id) {
  bool success = false;
  Mutex_lock(__idQueue.freeIdLock);
  if (__idQueue.size == __idQueue.cap) {
    int32_t newCap = __idQueue.cap << 1;
    int32_t* newIdList =
        realloc(__idQueue.freeIdList, newCap * sizeof(int32_t));
    if (newIdList) {
      __idQueue.cap = newCap;
    }
  }
  if (__idQueue.size < __idQueue.cap) {
    __idQueue.freeIdList[__idQueue.size++] = id;
    success = true;
  }
  Mutex_unlock(__idQueue.freeIdLock);
  return success;
}

static bool
__LocalId_pollQueue(int32_t* id) {
  *id = -1;
  Mutex_lock(__idQueue.freeIdLock);
  if (__idQueue.size > 0) {
    *id = __idQueue.freeIdList[--__idQueue.size];
  }
  Mutex_unlock(__idQueue.freeIdLock);
  return *id != -1;
}

bool
__ThreadLocal_offerId(int32_t id) {
  return __LocalId_offerQueue(id);
}

bool
__ThreadLocal_pollId(int32_t* id) {
  return __LocalId_pollQueue(id);
}
