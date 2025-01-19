//
// Created by 14049 on 2024/6/17.
//

#include <stdlib.h>
#include <xcl/lang/alloc.h>

static struct Pool* global_pool = NULL;

struct Pool* Pool_setGlobal(struct Pool* pool) {
  struct Pool* ret = global_pool;
  global_pool = pool;
  return ret;
}

void* Pool_alignMem(struct Pool* pool, unsigned size, unsigned alignment) {
  if (pool)
    return pool->__vptr->align_alloc(pool, size, alignment);
#ifdef Windows
  return _aligned_malloc(size, alignment);
#else
  void* ptr = NULL;
  int ret = posix_memalign(&ptr, alignment, size);
  if (ret)
    return NULL;
  return ptr;
#endif
}
void Pool_alignFree(struct Pool* pool, void* ptr) {
  if (pool)
    pool->__vptr->align_free(pool, ptr);
  else
#ifdef Windows
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void* palloc(unsigned size) { return Pool_alloc(global_pool, size); }
void pfree(void* ptr) { Pool_dealloc(global_pool, ptr); }
void* prealloc(void* ptr, unsigned size) { return Pool_realloc(global_pool, ptr, size); }
void* pcalloc(unsigned block, unsigned count) { return Pool_calloc(global_pool, block, count); }
void* palign(unsigned size, unsigned alignment) {
  return Pool_alignMem(global_pool, size, alignment);
}
void pfreea(void* ptr) { Pool_alignFree(global_pool, ptr); }
