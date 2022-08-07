//
// Created by xuyan on 2022/7/8.
//

#include "xcl/concurrent/GlobalLock.h"
#include <xcl/concurrent/CAtomic.h>

#include "xcl/concurrent/CMutex.h"
#include "xcl/lang/XclErr.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int32_t
__LocalId_genId();

typedef struct {
    int32_t* freeIdList;
    CMutex* freeIdLock;
    int32_t size;
    int32_t cap;
} __LocalIdQueue;

static __LocalIdQueue __idQueue;

static void
__LocalId_initQueue()
{
    memset(&__idQueue, 0, sizeof(__idQueue));
    __idQueue.cap = 8;
    int32_t* idList = (int32_t*)(malloc(__idQueue.cap * sizeof(int32_t)));
    if (idList)
    {
        CMutex* lock = Mutex_new();
        if (lock)
        {
            memset(idList, 0, sizeof(int32_t) * __idQueue.cap);
            __idQueue.freeIdLock = lock;
            __idQueue.freeIdList = idList;
            return;
        }
        free(idList);
    }
    memset(&__idQueue, 0, sizeof(__idQueue));
    assert(false);
}

static void
__LocalId_releaseQueue()
{
    if (__idQueue.freeIdList)
    {
        free(__idQueue.freeIdList);
        Mutex_delete(__idQueue.freeIdLock);
    }
    memset(&__idQueue, 0, sizeof(__idQueue));
}

static bool
__LocalId_offerQueue(int32_t id)
{
    bool success = false;
    Mutex_lock(__idQueue.freeIdLock);
    if (__idQueue.size == __idQueue.cap)
    {
        int32_t newCap = __idQueue.cap << 1;
        int32_t* newIdList = (int32_t*)(realloc(__idQueue.freeIdList,
                                                newCap * sizeof(int32_t)));
        if (newIdList)
        {
            __idQueue.cap = newCap;
            __idQueue.freeIdList = newIdList;
        }
        else
        {
            Err_set(XCL_MEMORY_ERR);
        }
    }
    if (__idQueue.size < __idQueue.cap)
    {
        __idQueue.freeIdList[__idQueue.size++] = id;
        success = true;
    }
    Mutex_unlock(__idQueue.freeIdLock);
    return success;
}

static bool
__LocalId_pollQueue(int32_t* id)
{
    *id = -1;
    Mutex_lock(__idQueue.freeIdLock);
    if (__idQueue.size > 0)
    {
        *id = __idQueue.freeIdList[--__idQueue.size];
    }
    Mutex_unlock(__idQueue.freeIdLock);
    return *id != -1;
}

static void
__LocalId_ensureQueue()
{
    static volatile bool initIdQueueDone = false;
    if (!initIdQueueDone)
    {
        __acquireGlobalLock();
        if (!initIdQueueDone)
        {
            __LocalId_initQueue();
            initIdQueueDone = true;
        }
        __releaseGlobalLock();
    }
}

int32_t
__LocalId_get()
{
    __LocalId_ensureQueue();
    int32_t id;
    if (__LocalId_pollQueue(&id))
    {
        return id;
    }
    return __LocalId_genId();
}

void
__LocalId_recycle(int32_t id)
{
    __LocalId_ensureQueue();
    __LocalId_offerQueue(id);
}

/*
 * we start gen thread local id from 1
 * because id 0 is reserved to store
 * thread handle
 */
static ATOMIC(int32_t) __LocalId_generator = 1;
#if CLANG || GNUC
#include <stdatomic.h>
int32_t
__LocalId_genId()
{
    int32_t id;
    id = atomic_fetch_add_explicit(&__LocalId_generator,
                                   1,
                                   memory_order_seq_cst);
    return id;
}
#elif defined(_MSC_VER)
#include <windows.h>
int32_t
__LocalId_genId()
{
    int32_t id;
    id = InterlockedAdd(&__LocalId_generator, 1);
    return id;
}
#endif
