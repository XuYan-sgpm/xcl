//
// Created by xuyan on 2022/7/8.
//

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/concurrent/CMutex.h"
#include "xcl/lang/XclErr.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int32_t __LocalId_genId();

typedef struct {
    int32_t* freeIdList;
    CMutex* freeIdLock;
    int32_t size;
    int32_t cap;
} __LocalIdQueue;

static __LocalIdQueue __idQueue;

static void __LocalId_initQueue()
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

static void __LocalId_releaseQueue()
{
    if (__idQueue.freeIdList)
    {
        free(__idQueue.freeIdList);
        Mutex_delete(__idQueue.freeIdLock);
    }
    memset(&__idQueue, 0, sizeof(__idQueue));
}

static bool __LocalId_offerQueue(int32_t id)
{
    bool success = false;
    Mutex_lock(__idQueue.freeIdLock);
    if (__idQueue.size == __idQueue.cap)
    {
        int32_t newCap = __idQueue.cap << 1;
        int32_t* newIdList =
            (int32_t*)(realloc(__idQueue.freeIdList, newCap * sizeof(int32_t)));
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

static bool __LocalId_pollQueue(int32_t* id)
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

int32_t __LocalId_get()
{
    int32_t id;
    if (__LocalId_pollQueue(&id))
    {
        return id;
    }
    return __LocalId_genId();
}

void __LocalId_recycle(int32_t id)
{
    __LocalId_offerQueue(id);
}

#ifdef __cplusplus
}
#endif

#ifdef ENABLE_CXX

#include <atomic>

static std::atomic<int32_t> __localIdGenerator(0);

namespace xcl
{
    namespace
    {
        class LocalIdInitializer
        {
        public:
            LocalIdInitializer();
        };

        xcl::LocalIdInitializer::LocalIdInitializer()
        {
            __LocalId_initQueue();
        }
    }// namespace
    static LocalIdInitializer __localIdInitializer;
}// namespace xcl

extern "C" int32_t __LocalId_genId()
{
    return __localIdGenerator.fetch_add(1, std::memory_order_seq_cst);
}

#elif GNUC || CLANG

#include <stdatomic.h>

static atomic_int_fast32_t __localIdGenerator = 0;

extern "C" int32_t __LocalId_genId()
{
    return atomic_fetch_add_explicit(&__localIdGenerator,
                                     1,
                                     memory_order_seq_cst);
}

static __attribute__((constructor)) void __LocalId_initialize()
{
    __LocalId_initQueue();
}

#else
#error "unsupported environment"
#endif
