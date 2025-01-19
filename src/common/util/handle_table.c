#include <internal/util/handle_pool.h>
#include <internal/util/handle_table.h>
#include <internal/util/rb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/comparators.h>
#include <xcl/util/hash_map.h>
#include <xcl/util/log.h>
#include <xcl/util/map.h>
#include <xcl/util/vector.h>

struct HandleTable {
  HandlePool* handle_pool;
  void* handle_map;
  Map* name_map;
  uintptr_t id_gen;
};

static inline void* HandleTable_makeHandle(uintptr_t id) { return (void*)(id << 2); }
static inline uintptr_t HandleTable_getId(void* h) { return (uintptr_t)h >> 2; }
static uintptr_t HandleTable_genId(HandleTable* handle_table) {
  static const uintptr_t handle_id_mask = ((uintptr_t)1 << (PTR_BITS - 2)) - 1;
  uintptr_t handle_id = ((++handle_table->id_gen) & handle_id_mask);
  if (!handle_id)
    handle_id = ((++handle_table->id_gen) & handle_id_mask);
  return handle_id;
}

static bool HandleTable_init(HandleTable* handle_table) {
  memset(handle_table, 0, sizeof(*handle_table));
  if ((handle_table->name_map = Map_new(64, sizeof(void*), scomp())) &&
      (handle_table->handle_map = HashMap_new(sizeof(uintptr_t), sizeof(void*), NULL,
                                              sizeof(void*) == 4 ? u32comp() : u64comp())))
    return true;
  Map_free(handle_table->name_map);
  return false;
}

HandleTable* HandleTable_new(HandlePool* pool) {
  HandleTable* handle_table = palloc(sizeof(HandleTable));
  if (handle_table && HandleTable_init(handle_table)) {
    handle_table->handle_pool = pool;
    return handle_table;
  }
  pfree(handle_table);
  return NULL;
}

void HandleTable_free(HandleTable* handle_table) {
  if (handle_table) {
    HandleTable_clear(handle_table);
    Map_free(handle_table->name_map);
    HashMap_free(handle_table->handle_map);
    pfree(handle_table);
  }
}

static void* HandleTable_mapHandle(HandleTable* handle_table, HandleEntry* e) {
  uintptr_t handle_id = HandleTable_genId(handle_table);
  HashIter hash_it = HashMap_add(handle_table->handle_map, &handle_id, &e);
  if (HashIter_valid(hash_it))
    return HandleTable_makeHandle(handle_id);
  return NULL;
}

static bool HandleTable_addNamedEntry(HandleTable* handle_table, const char* name, HandleEntry* e) {
  char key[64] = {0};
  snprintf(key, sizeof(key), name);
  TreeIter tree_it = Map_add(handle_table->name_map, true, key, &e);
  if (TreeIter_valid(tree_it)) {
    e->name = TreeIter_key(tree_it);
    return true;
  }
  return false;
}

void* HandleTable_map(HandleTable* handle_table, void* obj, void (*destructor)(void*),
                      const char* name) {
  if (!obj || (name && strlen(name) >= 64))
    return NULL;
  HandleEntry* e = HandlePool_alloc(handle_table->handle_pool);
  if (!e)
    return NULL;
  HandleEntry_init(e, obj, destructor);
  void* h = HandleTable_mapHandle(handle_table, e);
  if (h) {
    if (!name || !name[0] || HandleTable_addNamedEntry(handle_table, name, e))
      return h;
    uintptr_t handle_id = HandleTable_getId(h);
    LOG_ASSERT(HashMap_remove(handle_table->handle_map, &handle_id));
  }
  HandlePool_recycle(handle_table->handle_pool, e);
  return NULL;
}

static HandleEntry* HandleTable_getByHandle(HandleTable* handle_table, void* h) {
  uintptr_t handle_id = HandleTable_getId(h);
  HashIter it = HashMap_find(handle_table->handle_map, &handle_id);
  if (!HashIter_valid(it))
    return NULL;
  HandleEntry* e = *(HandleEntry**)HashIter_value(it);
  if (e->addr)
    return e;
  HashMap_removeAt(handle_table->handle_map, it);
  if (!--e->refs)
    HandlePool_recycle(handle_table->handle_pool, e);
  return NULL;
}

static HandleEntry* HandleTable_getByName(HandleTable* handle_table, const char* name) {
  if (!name || !name[0])
    return NULL;
  TreeIter tree_it = Map_find(handle_table->name_map, name);
  return !TreeIter_valid(tree_it) ? NULL : *(HandleEntry**)TreeIter_value(tree_it);
}

static void HandleTable_close0(HandleTable* handle_table, HashIter hash_it) {
  HandleEntry* e = *(HandleEntry**)HashIter_value(hash_it);
  LOG_ASSERT(e != NULL && e->refs > 0);
  if (!e->addr) {
    if (!--e->refs)
      HandlePool_recycle(handle_table->handle_pool, e);
  } else {
    const char* name = e->name;
    if (HandleEntry_close(e)) {
      LOG_ASSERT(!name || Map_remove(handle_table->name_map, name));
      HandlePool_recycle(handle_table->handle_pool, e);
    }
  }
  LOG_ASSERT(HashMap_removeAt(handle_table->handle_map, hash_it));
}

void HandleTable_close(HandleTable* handle_table, void* h) {
  uintptr_t handle_id = HandleTable_getId(h);
  HashIter it = HashMap_find(handle_table->handle_map, &handle_id);
  if (HashIter_valid(it))
    HandleTable_close0(handle_table, it);
}

void* HandleTable_get(HandleTable* handle_table, void* h) {
  HandleEntry* e = HandleTable_getByHandle(handle_table, h);
  return e ? e->addr : NULL;
}

static void* HandleTable_clone0(HandleTable* handle_table, HandleEntry* e) {
  if (!e)
    return NULL;
  uintptr_t handle_id = HandleTable_genId(handle_table);
  HashIter it = HashMap_add(handle_table->handle_map, &handle_id, &e);
  if (!HashIter_valid(it))
    return NULL;
  e->refs++;
  return HandleTable_makeHandle(handle_id);
}

void* HandleTable_clone(HandleTable* handle_table, void* h) {
  return HandleTable_clone0(handle_table, HandleTable_getByHandle(handle_table, h));
}

void* HandleTable_cloneByName(HandleTable* handle_table, const char* name) {
  return HandleTable_clone0(handle_table, HandleTable_getByName(handle_table, name));
}

bool HandleTable_contains(HandleTable* handle_table, const char* name) {
  return HandleTable_getByName(handle_table, name);
}

void HandleTable_closeByName(HandleTable* handle_table, const char* name) {
  HandleEntry* e = HandleTable_getByName(handle_table, name);
  if (e) {
    HandleEntry_deleteObject(e);
    Map_remove(handle_table->name_map, name);
  }
}

void HandleTable_clear(HandleTable* handle_table) {
  while (!HashMap_empty(handle_table->handle_map)) {
    HandleTable_close0(handle_table, HashMap_begin(handle_table->handle_map));
  }
}
