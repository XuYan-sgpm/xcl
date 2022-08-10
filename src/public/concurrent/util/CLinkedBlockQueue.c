//
// Created by xuyan on 2022/7/5.
//

#include "xcl/concurrent/util/CLinkedBlockQueue.h"
#include "xcl/concurrent/CMutex.h"
#include "xcl/concurrent/CCond.h"
#include "xcl/concurrent/CAtomic.h"
#include <assert.h>
#include <string.h>

typedef struct _Node {
    struct _Node* next;
    char attach[0];
} Node;

struct _CLinkedBlockingQueue_st {
    CMutex* const putLock;
    CMutex* const popLock;
    CCond* const notEmpty;
    CCond* const notFull;
    CPool* const pool;
    const uint32_t eleSize;
    ALIGNED(4) ATOMIC(int32_t) size;
    const uint32_t limit;
    Node* tail;
    Node header;
};

static inline void
__LinkedBlockingQueue_emptyDel(CLinkedBlockingQueue* queue)
{
    Mutex_delete(queue->putLock);
    Mutex_delete(queue->popLock);
    Cond_delete(queue->notEmpty);
    Cond_delete(queue->notFull);
    Pool_dealloc(queue->pool, queue, sizeof(CLinkedBlockingQueue));
}

static inline Node*
__LinkedBlockingQueue_allocNode(CLinkedBlockingQueue* queue)
{
    return Pool_alloc(queue->pool, sizeof(Node) + queue->eleSize);
}

static inline void
__LinkedBlockingQueue_deallocNode(CLinkedBlockingQueue* queue, Node* node)
{
    Pool_dealloc(queue->pool, node, sizeof(Node) + queue->eleSize);
}

XCL_EXPORT CLinkedBlockingQueue* XCL_API
LinkedBlockingQueue_new(uint32_t es, uint32_t limit)
{
    return LinkedBlockingQueue_newByPool(es, limit, Pool_def());
}

XCL_EXPORT CLinkedBlockingQueue* XCL_API
LinkedBlockingQueue_newByPool(uint32_t es, uint32_t limit, CPool* pool)
{
    CLinkedBlockingQueue* queue
        = Pool_alloc(pool, sizeof(CLinkedBlockingQueue));
    if (queue)
    {
        CLinkedBlockingQueue q = {.putLock = Mutex_new(),
                                  .popLock = Mutex_new(),
                                  .notEmpty = Cond_new(),
                                  .notFull = Cond_new(),
                                  .pool = pool,
                                  .eleSize = es,
                                  .size = 0,
                                  .limit = limit,
                                  .tail = 0,
                                  .header = {0}};
        q.tail = &q.header;
        memcpy((void*)queue, (void*)&q, sizeof(CLinkedBlockingQueue));
        if (q.putLock && q.popLock && q.notEmpty && q.notFull)
        {
            return queue;
        }
        __LinkedBlockingQueue_emptyDel(queue);
    }
    return 0;
}

XCL_EXPORT void XCL_API
LinkedBlockingQueue_delete(CLinkedBlockingQueue* queue,
                           void (*destructor)(void*, void*),
                           void* usr)
{
    Mutex_lock(queue->putLock);
    Mutex_lock(queue->popLock);
    Node* cur = queue->header.next;
    while (cur)
    {
        Node* next = cur->next;
        destructor(usr, cur->attach);
        __LinkedBlockingQueue_deallocNode(queue, cur);
        cur = next;
        __Atomic_fetchAdd32(&queue->size, -1, memory_order_seq_cst);
    }
    assert(__Atomic_load32(&queue->size, memory_order_seq_cst) == 0);
    Mutex_unlock(queue->popLock);
    Mutex_unlock(queue->putLock);
    __LinkedBlockingQueue_emptyDel(queue);
}

XCL_EXPORT bool XCL_API
LinkedBlockingQueue_offer(CLinkedBlockingQueue* queue,
                          const void* src,
                          bool (*constructor)(void*, void*, const void*),
                          void* usr)
{
    Node* node = __LinkedBlockingQueue_allocNode(queue);
    if (!node)
    {
        return false;
    }
    if (!constructor(usr, node->attach, src))
    {
        __LinkedBlockingQueue_deallocNode(queue, node);
        return false;
    }
    bool ret = false;
    bool notifyNotFull = false;
    bool notifyNotEmpty = false;
    Mutex_lock(queue->putLock);
    int32_t size;
    if ((size = __Atomic_load32(&queue->size, memory_order_acquire))
        == queue->limit)
    {
    }
    else
    {
        ret = true;
        queue->tail->next = node;
        queue->tail = node;
        size = __Atomic_fetchAdd32(&queue->size, 1, memory_order_acq_rel);
        if (size + 1 < queue->limit)
        {
            notifyNotFull = true;
        }
    }
    Mutex_unlock(queue->putLock);
    if (notifyNotFull)
    {
        Cond_signal(queue->notFull);
    }
    if (ret && size == 0)
    {
        Cond_signal(queue->notEmpty);
    }
    return ret;
}

XCL_EXPORT bool XCL_API
LinkedBlockingQueue_peek(CLinkedBlockingQueue* queue,
                         bool (*constructor)(void*, void*, const void*),
                         void* usr,
                         void* dst)
{
    bool ret = false;
    Mutex_lock(queue->popLock);
    if (__Atomic_load32(&queue->size, memory_order_acquire) == 0)
    {
    }
    else
    {
        Node* first = queue->header.next;
        assert(first);
        ret = constructor(usr, dst, first->attach);
    }
    Mutex_unlock(queue->popLock);
    return ret;
}

XCL_EXPORT bool XCL_API
LinkedBlockingQueue_poll(CLinkedBlockingQueue* queue,
                         bool (*move)(void*, void*, void*),
                         void* usr,
                         void* dst)
{
    bool ret = false;
    bool notifyNotFull = false;
    bool notifyNotEmpty = false;
    Mutex_lock(queue->popLock);
    int32_t size;
    if ((size = __Atomic_load32(&queue->size, memory_order_acquire)) == 0)
    {
        if (queue->limit == 1)
        {
            notifyNotFull = true;
        }
    }
    else
    {
        Node* first = queue->header.next;
        assert(first);
        ret = move(usr, dst, first->attach);
        queue->header.next = first->next;
        size = __Atomic_fetchAdd32(&queue->size, -1, memory_order_acq_rel);
        __LinkedBlockingQueue_deallocNode(queue, first);
        if (queue->limit >= 0 && size == queue->limit)
        {
            notifyNotFull = true;
        }
        if (size > 1)
        {
            notifyNotEmpty = true;
        }
    }
    Mutex_unlock(queue->popLock);
    if (notifyNotFull)
    {
        Cond_signal(queue->notFull);
    }
    if (notifyNotEmpty)
    {
        Cond_signal(queue->notEmpty);
    }
    return ret;
}

XCL_EXPORT bool XCL_API
LinkedBlockingQueue_isEmpty(CLinkedBlockingQueue* queue)
{
    Mutex_lock(queue->putLock);
    Mutex_lock(queue->popLock);
    bool ret = queue->header.next
               && __Atomic_load32(&queue->size, memory_order_acquire) > 0;
    Mutex_unlock(queue->popLock);
    Mutex_unlock(queue->putLock);
    return ret;
}

XCL_EXPORT int32_t XCL_API
LinkedBlockingQueue_size(CLinkedBlockingQueue* queue)
{
    return __Atomic_load32(&queue->size, memory_order_acq_rel);
}
