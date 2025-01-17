// //
// // Created by 徐琰 on 2023/10/22.
// //

// #include <xcl/concurrent/util/concurrent_ring_queue.h>
// #include <internal/concurrent/atomic.h>
// #include <xcl/lang/system.h>
// #include <string.h>
// #include <xcl/lang/alloc.h>

// typedef struct {
//     volatile uint64_t* data;
//     volatile int start;
//     volatile int finish;
//     const unsigned cap;
// } _CRQ;

// static inline always_inline bool __CRQ_checkCapacity(unsigned cap)
// {
//     return (cap & (cap - 1)) == 0;
// }
// static inline always_inline unsigned __CRQ_map(_CRQ* crq, unsigned idx)
// {
//     return idx & (crq->cap - 1);
// }

// void* CRQ_new(unsigned cap)
// {
//     if (!__CRQ_checkCapacity(cap))
//         return NULL;
//     _CRQ* crq = palign(64, 64);
//     if (!crq)
//         return NULL;
//     memset(crq, 0, 64);
//     memcpy((void*)&crq->cap, &cap, sizeof(cap));
//     crq->data = palign(cap * sizeof(uint64_t), sizeof(uint64_t));
//     if (!crq->data)
//     {
//         pfreea(crq);
//         return NULL;
//     }
//     memset((void*)crq->data, 0xff, cap * sizeof(uint64_t));
//     return crq;
// }

// void CRQ_delete(void* q)
// {
//     _CRQ* crq = q;
//     pfreea((void*)crq->data);
//     pfreea(crq);
// }

// bool CRQ_offer(void* q, uint64_t a)
// {
//     if (a == _INVALID_QUEUE_ELEM)
//         return false;
//     _CRQ* crq = q;
//     int end = __atomic_load32(&crq->finish, ACQUIRE);
//     for (;;)
//     {
//         int beg = __atomic_load32(&crq->start, ACQUIRE);
//         if (end - beg >= crq->cap)
//         {
//             return false;
//         }
//         uint64_t elem
//             = __atomic_load64(&crq->data[__CRQ_map(crq, end)], ACQUIRE);
//         if (elem != _INVALID_QUEUE_ELEM)
//         {
//             end = __atomic_load32(&crq->finish, ACQUIRE);
//             continue;
//         }
//         if (__atomic_cas32(&crq->finish, false, &end, end + 1, ACQ_REL))
//         {
//             __atomic_store64(&crq->data[__CRQ_map(crq, end)], a, RELEASE);
//             return true;
//         }
//     }
// }

// bool CRQ_poll(void* q, uint64_t* val)
// {
//     _CRQ* crq = q;
//     int beg = __atomic_load32(&crq->start, ACQUIRE);
//     for (;;)
//     {
//         int end = __atomic_load32(&crq->finish, ACQUIRE);
//         if (end - beg == 0)
//         {
//             return false;
//         }
//         int64_t elem
//             = __atomic_load64(&crq->data[__CRQ_map(crq, beg)], ACQUIRE);
//         if (elem == _INVALID_QUEUE_ELEM)
//         {
//             beg = __atomic_load32(&crq->start, ACQUIRE);
//             continue;
//         }
//         *val = elem;
//         if (__atomic_cas32(&crq->start, false, &beg, beg + 1, ACQ_REL))
//         {
//             elem = _INVALID_QUEUE_ELEM;
//             __atomic_store64(&crq->data[__CRQ_map(crq, beg)], elem, RELEASE);
//             return true;
//         }
//     }
// }

// unsigned CRQ_size(void* q)
// {
//     _CRQ* crq = q;
//     return __atomic_load32(&crq->start, ACQUIRE)
//            - __atomic_load32(&crq->finish, ACQUIRE);
// }

// bool CRQ_empty(void* q)
// {
//     return CRQ_size(q) == 0;
// }

// void CRQ_clear(void* q)
// {
//     uint64_t val;
//     while (CRQ_poll(q, &val))
//     {
//     }
// }
