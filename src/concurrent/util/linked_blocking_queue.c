//
// Created by xuyan on 2022/7/5.
//

#include "concurrent/util/linked_blocking_queue.h"
#include "concurrent/mutex.h"
#include "concurrent/cond.h"
#include "concurrent/xcl_atomic.h"
#include <assert.h>
#include <string.h>

typedef struct Node {
    struct Node* next;
    char attach[0];
} Node;

struct LinkedBlockingQueue {
    Mutex* const put_lock;
    Mutex* const pop_lock;
    Cond* const not_empty;
    Cond* const not_full;
    struct Pool* const pool;
    const uint32_t element_size;
    ALIGNED(4) ATOMIC(int32_t) size;
    const uint32_t limit;
    Node* tail;
    Node header;
};

static inline void
__LinkedBlockingQueue_emptyDel(LinkedBlockingQueue* queue)
{
    Mutex_delete(queue->put_lock);
    Mutex_delete(queue->pop_lock);
    Cond_delete(queue->not_empty);
    Cond_delete(queue->not_full);
    Pool_dealloc(queue->pool, queue, sizeof(LinkedBlockingQueue));
}

static inline Node*
__LinkedBlockingQueue_allocNode(LinkedBlockingQueue* queue)
{
    return Pool_alloc(queue->pool, sizeof(Node) + queue->element_size);
}

static inline void
__LinkedBlockingQueue_deallocNode(LinkedBlockingQueue* queue, Node* node)
{
    Pool_dealloc(queue->pool, node, sizeof(Node) + queue->element_size);
}

XCL_EXPORT LinkedBlockingQueue* XCL_API
LinkedBlockingQueue_new(uint32_t es, uint32_t limit)
{
    return LinkedBlockingQueue_newByPool(es, limit, Pool_def());
}

XCL_EXPORT LinkedBlockingQueue* XCL_API
LinkedBlockingQueue_newByPool(uint32_t es, uint32_t limit, struct Pool* pool)
{
    LinkedBlockingQueue* queue = Pool_alloc(pool, sizeof(LinkedBlockingQueue));
    if (queue)
    {
        LinkedBlockingQueue q = {.put_lock = Mutex_new(),
                                 .pop_lock = Mutex_new(),
                                 .not_empty = Cond_new(),
                                 .not_full = Cond_new(),
                                 .pool = pool,
                                 .element_size = es,
                                 .size = 0,
                                 .limit = limit,
                                 .tail = 0,
                                 .header = {0}};
        q.tail = &q.header;
        memcpy((void*)queue, (void*)&q, sizeof(LinkedBlockingQueue));
        if (q.put_lock && q.pop_lock && q.not_empty && q.not_full)
        {
            return queue;
        }
        __LinkedBlockingQueue_emptyDel(queue);
    }
    return 0;
}

XCL_EXPORT void XCL_API
LinkedBlockingQueue_delete(LinkedBlockingQueue* queue,
                           void (*destructor)(void*, void*),
                           void* obj)
{
    Mutex_lock(queue->put_lock);
    Mutex_lock(queue->pop_lock);
    Node* cur = queue->header.next;
    while (cur)
    {
        Node* next = cur->next;
        destructor(obj, cur->attach);
        __LinkedBlockingQueue_deallocNode(queue, cur);
        cur = next;
        __Atomic_fetchAdd32(&queue->size, -1, memory_order_seq_cst);
    }
    assert(__Atomic_load32(&queue->size, memory_order_seq_cst) == 0);
    Mutex_unlock(queue->pop_lock);
    Mutex_unlock(queue->put_lock);
    __LinkedBlockingQueue_emptyDel(queue);
}

XCL_EXPORT bool XCL_API
LinkedBlockingQueue_offer(LinkedBlockingQueue* queue,
                          const void* src,
                          bool (*constructor)(void*, void*, const void*),
                          void* obj)
{
    Node* node = __LinkedBlockingQueue_allocNode(queue);
    if (!node)
    {
        return false;
    }
    if (!constructor(obj, node->attach, src))
    {
        __LinkedBlockingQueue_deallocNode(queue, node);
        return false;
    }
    bool ret = false;
    bool notifyNotFull = false;
    bool notifyNotEmpty = false;
    Mutex_lock(queue->put_lock);
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
    Mutex_unlock(queue->put_lock);
    if (notifyNotFull)
    {
        Cond_signal(queue->not_full);
    }
    if (ret && size == 0)
    {
        Cond_signal(queue->not_empty);
    }
    return ret;
}

XCL_EXPORT bool XCL_API
LinkedBlockingQueue_peek(LinkedBlockingQueue* queue,
                         bool (*constructor)(void*, void*, const void*),
                         void* obj,
                         void* dst)
{
    bool ret = false;
    Mutex_lock(queue->pop_lock);
    if (__Atomic_load32(&queue->size, memory_order_acquire) == 0)
    {
    }
    else
    {
        Node* first = queue->header.next;
        assert(first);
        ret = constructor(obj, dst, first->attach);
    }
    Mutex_unlock(queue->pop_lock);
    return ret;
}

XCL_EXPORT bool XCL_API
LinkedBlockingQueue_poll(LinkedBlockingQueue* queue,
                         bool (*move)(void*, void*, void*),
                         void* obj,
                         void* dst)
{
    bool ret = false;
    bool notifyNotFull = false;
    bool notifyNotEmpty = false;
    Mutex_lock(queue->pop_lock);
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
        ret = move(obj, dst, first->attach);
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
    Mutex_unlock(queue->pop_lock);
    if (notifyNotFull)
    {
        Cond_signal(queue->not_full);
    }
    if (notifyNotEmpty)
    {
        Cond_signal(queue->not_empty);
    }
    return ret;
}

XCL_EXPORT bool XCL_API
LinkedBlockingQueue_isEmpty(LinkedBlockingQueue* queue)
{
    Mutex_lock(queue->put_lock);
    Mutex_lock(queue->pop_lock);
    bool ret = queue->header.next
               && __Atomic_load32(&queue->size, memory_order_acquire) > 0;
    Mutex_unlock(queue->pop_lock);
    Mutex_unlock(queue->put_lock);
    return ret;
}

XCL_EXPORT int32_t XCL_API
LinkedBlockingQueue_size(LinkedBlockingQueue* queue)
{
    return __Atomic_load32(&queue->size, memory_order_acq_rel);
}
