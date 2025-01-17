#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HandleEntry HandleEntry;
struct HandleEntry {
  void* addr;
  void (*destructor)(void*);
  uintptr_t refs;
  HandleEntry* next;
  const char* name;
};

void HandleEntry_init(HandleEntry* obj, void* addr, void (*destructor)(void*));
bool HandleEntry_close(HandleEntry* obj);
void HandleEntry_deleteObject(HandleEntry* e);

typedef struct HandlePool HandlePool;
HandlePool* HandlePool_new();
void HandlePool_free(HandlePool* pool);
HandleEntry* HandlePool_alloc(HandlePool* pool);
void HandlePool_recycle(HandlePool* pool, HandleEntry* e);

#ifdef __cplusplus
}
#endif
