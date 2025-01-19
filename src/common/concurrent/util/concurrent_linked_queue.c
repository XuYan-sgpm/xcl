// //
// // Created by 徐琰 on 2023/10/19.
// //

// #include <xcl/concurrent/util/concurrent_linked_queue.h>
// #include <xcl/lang/system.h>
// #include <string.h>
// #include <stddef.h>
// #include <xcl/lang/define.h>
// #include <stdlib.h>
// #include <internal/concurrent/atomic.h>
// #include <xcl/lang/alloc.h>

// typedef struct _CLNode _CLNode;
// typedef struct {
//     _CLNode *head, *tail;
//     uint32_t size;
//     int push_lock, pop_lock;
// } _CLQ;

// struct _CLNode {
//     _CLNode* next;
//     uint64_t attach;
// };

// static _CLNode* __CLNode_new(uint64_t a)
// {
//     _CLNode* node = palign(sizeof(_CLNode), PTR_SIZE);
//     if (node)
//     {
//         memset(node, 0, sizeof(*node));
//         node->attach = a;
//         node->next = node;
//     }
//     return node;
// }

// void* CLQ_new()
// {
//     _CLQ* clq = palign(64, PTR_SIZE);
//     if (!clq || ((uintptr_t)clq & (PTR_SIZE - 1)))
//     {
//         pfreea(clq);
//         return NULL;
//     }
//     memset(clq, 0, 64);
//     __atomic_init_flag(&clq->push_lock);
//     __atomic_init_flag(&clq->pop_lock);
//     _CLNode* node = __CLNode_new(0);
//     if (!node)
//     {
//         pfreea(clq);
//         return NULL;
//     }
//     clq->head = clq->tail = node;
//     return clq;
// }

// void CLQ_delete(void* q)
// {
//     _CLQ* clq = q;
//     CLQ_clear(q);
//     pfreea(clq->head);
//     pfreea(q);
// }

// bool CLQ_offer(void* q, uint64_t a)
// {
//     _CLQ* clq = q;
//     _CLNode* node = __CLNode_new(a);
//     if (node)
//     {
//         while (__atomic_tas(&clq->push_lock, ACQ_REL))
//         {
//         }
//         __atomic_store_ptr(&clq->tail->next, node, RELEASE);
//         clq->tail = node;
//         __atomic_reset(&clq->push_lock, RELEASE);
//         __atomic_fetch_add32(&clq->size, 1, ACQ_REL);
//     }
//     return node;
// }

// bool CLQ_poll(void* q, uint64_t* val)
// {
//     _CLQ* clq = q;
//     bool ret = false;
//     while (__atomic_tas(&clq->pop_lock, ACQ_REL))
//     {
//     }
//     _CLNode* h = clq->head;
//     _CLNode* h_next = __atomic_load_ptr(&h->next, ACQUIRE);
//     if (h_next != h)
//     {
//         *val = h_next->attach;
//         clq->head = h_next;
//         ret = true;
//     }
//     __atomic_reset(&clq->pop_lock, RELEASE);
//     if (ret)
//     {
//         __atomic_fetch_sub32(&clq->size, 1, ACQ_REL);
//         pfreea(h);
//     }
//     return ret;
// }

// unsigned CLQ_size(void* q)
// {
//     _CLQ* clq = q;
//     return __atomic_load32(&clq->size, ACQUIRE);
// }

// bool CLQ_empty(void* q)
// {
//     return CLQ_size(q) == 0;
// }

// void CLQ_clear(void* q)
// {
//     uint64_t val;
//     while (CLQ_poll(q, &val))
//     {
//     }
// }
