//
// Created by 徐琰 on 2023/10/17.
//

#include <internal/lang/local_reserve_id.h>
#include <internal/lang/local_storage.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/lang/define.h>
#include <xcl/lang/thread_local.h>
#include <xcl/util/log.h>

LocalPair LocalPair_make(void* a, void (*f)(void*)) {
  LocalPair o = {.attach = a, .destructor = f};
  return o;
}

typedef struct {
  bool lock_usr_region;
  uintptr_t cap;
  LocalPair* pairs;
} LocalStorage;

void* LocalStorage_new() {
  static const uintptr_t initial_ls_cap = 64;
  LocalStorage* ls = malloc(sizeof(LocalStorage));
  if (ls && (ls->pairs = malloc(sizeof(LocalPair) * initial_ls_cap))) {
    memset(ls->pairs, 0, sizeof(LocalPair) * initial_ls_cap);
    ls->cap = initial_ls_cap;
    ls->lock_usr_region = false;
    return ls;
  }
  free(ls);
  return NULL;
}

static bool LocalStorage_expand(LocalStorage* ls, uintptr_t n) {
  uintptr_t new_cap = ls->cap;
  while (new_cap < n)
    new_cap <<= 1;
  LocalPair* new_pairs = realloc(ls->pairs, sizeof(LocalPair) * new_cap);
  if (!new_pairs)
    return false;
  memset(&new_pairs[ls->cap], 0, sizeof(LocalPair) * (new_cap - ls->cap));
  ls->pairs = new_pairs;
  ls->cap = new_cap;
  return true;
}

static void LocalStorage_clean(void* obj, uintptr_t l, uintptr_t r) {
  LocalStorage* ls = obj;
  while (l < r) {
    LocalPair pair = ls->pairs[l++];
    if (pair.destructor)
      pair.destructor(pair.attach);
  }
}
void LocalStorage_free(void* obj) {
  if (obj) {
    LocalStorage* ls = obj;
    ls->lock_usr_region = true;
    LocalStorage_clean(ls, LOCAL_ID_MAX_RESERVED, ls->cap);
    LocalStorage_clean(ls, 0, LOCAL_ID_MAX_RESERVED);
    free(ls->pairs);
    free(ls);
  }
}

bool LocalStorage_set(void* obj, uintptr_t id, LocalPair pair) {
  LocalStorage* ls = obj;
  if (ls->lock_usr_region && id >= LOCAL_ID_MAX_RESERVED)
    LOG_ASSERT_MSG(LOG_ERROR, "local storage [%d-%d)] is locked\n", LOCAL_ID_MAX_RESERVED, ls->cap);
  if (id >= ls->cap && !LocalStorage_expand(ls, id + 1))
    return false;
  return (ls->pairs[id] = pair, true);
}

LocalPair* LocalStorage_get(void* obj, uintptr_t id) {
  LocalStorage* ls = obj;
  if (id >= ls->cap && !LocalStorage_expand(ls, id + 1))
    return NULL;
  return &ls->pairs[id];
}
