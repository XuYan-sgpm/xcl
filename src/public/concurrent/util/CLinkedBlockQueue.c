//
// Created by xuyan on 2022/7/5.
//

#include "xcl/concurrent/util/CLinkedBlockQueue.h"
#include "xcl/concurrent/CMutex.h"
#include "xcl/concurrent/CCond.h"
#include "xcl/concurrent/CInternalAtomic.h"

struct _CLinkedBlockingQueue_st {
    CMutex* putLock;
    CMutex* popLock;
    CCond* notEmpty;
    CCond* notFull;
    const uint32_t eleSize;
    void* blocks;
    ALIGNED(4) ATOMIC(int32_t) blockCount;
    const uint32_t queueCap;
};
