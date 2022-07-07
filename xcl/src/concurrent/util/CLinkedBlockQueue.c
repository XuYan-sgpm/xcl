//
// Created by xuyan on 2022/7/5.
//

#include "concurrent/util/CLinkedBlockQueue.h"
#include <concurrent/CMutex.h>
#include <stdatomic.h>

struct CLinkedBlockingQueue_st {
  void* putLock;
  void* popLock;
  void* notEmpty;
  void* notFull;
  const uint32_t blockSize;
  void* blocks;
  atomic_int_fast32_t blockCount;
  const uint32_t queueCap;
};
