#pragma once

#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ObjectPool ObjectPool;
struct ObjectPool {
  struct {
    void* (*alloc_obj)(ObjectPool*);
    void (*recycle_obj)(ObjectPool*, void*);
  }* __vptr;
};

inline always_inline void* ObjectPool_alloc(ObjectPool* pool) {
  return pool->__vptr->alloc_obj(pool);
}
inline always_inline void ObjectPool_recycle(ObjectPool* pool, void* obj) {
  pool->__vptr->recycle_obj(pool, obj);
}

#ifdef __cplusplus
}
#endif
