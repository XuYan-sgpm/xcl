//
// Created by xuyan on 2022/7/8.
//

#include <concurrent/xcl_atomic.h>

#include "concurrent/mutex.h"
#include "lang/xcl_err.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int32_t
__LocalId_genId();

typedef struct {
    int32_t* free_id_list;
    Mutex* free_id_lock;
    int32_t size;
    int32_t cap;
} LocalIdQueue;

static LocalIdQueue __id_queue;

static void
__LocalId_initQueue()
{
    memset(&__id_queue, 0, sizeof(__id_queue));
    __id_queue.cap = 8;
    int32_t* id_list = (int32_t*)(malloc(__id_queue.cap * sizeof(int32_t)));
    if (id_list)
    {
        Mutex* lock = Mutex_new();
        if (lock)
        {
            memset(id_list, 0, sizeof(int32_t) * __id_queue.cap);
            __id_queue.free_id_lock = lock;
            __id_queue.free_id_list = id_list;
            return;
        }
        free(id_list);
    }
    memset(&__id_queue, 0, sizeof(__id_queue));
    assert(false);
}

static void
__LocalId_releaseQueue()
{
    if (__id_queue.free_id_list)
    {
        free(__id_queue.free_id_list);
        Mutex_delete(__id_queue.free_id_lock);
    }
    memset(&__id_queue, 0, sizeof(__id_queue));
}

static bool
__LocalId_offerQueue(int32_t id)
{
    bool success = false;
    Mutex_lock(__id_queue.free_id_lock);
    if (__id_queue.size == __id_queue.cap)
    {
        int32_t newCap = __id_queue.cap << 1;
        int32_t* newIdList = (int32_t*)(realloc(__id_queue.free_id_list,
                                                newCap * sizeof(int32_t)));
        if (newIdList)
        {
            __id_queue.cap = newCap;
            __id_queue.free_id_list = newIdList;
        }
        else
        {
            Err_set(XCL_MEMORY_ERR);
        }
    }
    if (__id_queue.size < __id_queue.cap)
    {
        __id_queue.free_id_list[__id_queue.size++] = id;
        success = true;
    }
    Mutex_unlock(__id_queue.free_id_lock);
    return success;
}

static bool
__LocalId_pollQueue(int32_t* id)
{
    *id = -1;
    Mutex_lock(__id_queue.free_id_lock);
    if (__id_queue.size > 0)
    {
        *id = __id_queue.free_id_list[--__id_queue.size];
    }
    Mutex_unlock(__id_queue.free_id_lock);
    return *id != -1;
}

static void
__LocalId_ensureQueue()
{
    // static volatile bool initIdQueueDone = false;
    // if (!initIdQueueDone)
    // {
    //     __acquireGlobalLock();
    //     if (!initIdQueueDone)
    //     {
    //         __LocalId_initQueue();
    //         initIdQueueDone = true;
    //     }
    //     __releaseGlobalLock();
    // }
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
int32_t
__LocalId_genId()
{
    return __Atomic_fetchAdd32(&__LocalId_generator, 1, memory_order_seq_cst);
}
