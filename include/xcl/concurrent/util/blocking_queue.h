//
// Created by 14049 on 2024/5/14.
//

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BlockingQueue BlockingQueue;
struct BlockingQueue {
  struct {
    bool (*offer)(BlockingQueue*, const void*);
    bool (*put)(BlockingQueue*, const void*);
    bool (*put_timeout)(BlockingQueue*, const void*, int);
    bool (*poll)(BlockingQueue*, void*);
    void (*take)(BlockingQueue*, void*);
    bool (*take_timeout)(BlockingQueue*, void*, int);
    void (*clear)(BlockingQueue*);
    void* (*peek)(BlockingQueue*);
    int (*size)(BlockingQueue*);
    bool (*empty)(BlockingQueue*);
  }* __vptr;
};

inline always_inline bool BlockingQueue_offer(BlockingQueue* bq, const void* value) {
  return bq->__vptr->offer(bq, value);
}
inline always_inline bool BlockingQueue_put(BlockingQueue* bq, const void* value) {
  return bq->__vptr->put(bq, value);
}
inline always_inline bool BlockingQueue_putTimeout(BlockingQueue* bq, const void* value,
                                                   int millis) {
  return bq->__vptr->put_timeout(bq, value, millis);
}
inline always_inline void BlockingQueue_take(BlockingQueue* bq, void* value) {
  bq->__vptr->take(bq, value);
}
inline always_inline bool BlockingQueue_takeTimeout(BlockingQueue* bq, void* value, int millis) {
  return bq->__vptr->take_timeout(bq, value, millis);
}
inline always_inline void BlockingQueue_clear(BlockingQueue* bq) { bq->__vptr->clear(bq); }
inline always_inline bool BlockingQueue_poll(BlockingQueue* bq, void* value) {
  return bq->__vptr->poll(bq, value);
}
inline always_inline void* BlockingQueue_peek(BlockingQueue* bq) { return bq->__vptr->peek(bq); }
inline always_inline int BlockingQueue_size(BlockingQueue* bq) { return bq->__vptr->size(bq); }
inline always_inline bool BlockingQueue_empty(BlockingQueue* bq) { return bq->__vptr->empty(bq); }

#ifdef __cplusplus
}
#endif
