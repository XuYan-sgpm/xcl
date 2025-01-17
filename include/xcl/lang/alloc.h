//
// Created by 14049 on 2024/6/17.
//

#pragma once

#include <stdlib.h>
#include <xcl/lang/define.h>
#ifdef __cplusplus
extern "C" {
#endif

struct Pool {
  struct {
    void* (*alloc)(struct Pool*, unsigned);
    void (*dealloc)(struct Pool*, void*);
    void* (*realloc)(struct Pool*, void*, unsigned);
    void* (*calloc)(struct Pool*, unsigned, unsigned);
    void* (*align_alloc)(struct Pool*, unsigned, unsigned);
    void (*align_free)(struct Pool*, void*);
  }* __vptr;
};

inline always_inline void* Pool_alloc(struct Pool* pool, unsigned size) {
  return pool ? pool->__vptr->alloc(pool, size) : malloc(size);
}
inline always_inline void Pool_dealloc(struct Pool* pool, void* ptr) {
  pool ? pool->__vptr->dealloc(pool, ptr) : free(ptr);
}
inline always_inline void* Pool_realloc(struct Pool* pool, void* ptr, unsigned size) {
  return pool ? pool->__vptr->realloc(pool, ptr, size) : realloc(ptr, size);
}
inline always_inline void* Pool_calloc(struct Pool* pool, unsigned block, unsigned count) {
  return pool ? pool->__vptr->calloc(pool, block, count) : calloc(count, block);
}
void* Pool_alignMem(struct Pool* pool, unsigned size, unsigned alignment);
void Pool_alignFree(struct Pool* pool, void* ptr);
struct Pool* Pool_setGlobal(struct Pool* pool);
void* palloc(unsigned size);
void pfree(void* ptr);
void* prealloc(void* ptr, unsigned size);
void* pcalloc(unsigned block, unsigned count);
void* palign(unsigned size, unsigned alignment);
void pfreea(void* ptr);

#ifdef __cplusplus
}
#endif
