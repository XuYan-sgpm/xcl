//
// Created by 徐琰 on 2023/10/17.
//

#include <internal/lang/local_reserve_id.h>
#include <internal/lang/local_storage.h>
#include <internal/lang/os_local.h>
#include <internal/xcl.h>
#include <stddef.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/lang/system.h>
#include <xcl/lang/thread_local.h>
#include <xcl/util/log.h>
#include <xcl/util/vector.h>

static Vector* local_id_vec = NULL;
static void Local_freeIdVec() { Vec_free(local_id_vec); }

static int Local_allocId() {
  static int local_id_gen = 0;
  if (!local_id_vec) {
    if (!(local_id_vec = Vec_new(sizeof(int))))
      return -1;
    postExit(Local_freeIdVec);
  }
  if (!Vec_empty(local_id_vec)) {
    int id;
    Vec_pop(local_id_vec, &id);
    return id;
  }
  return local_id_gen++;
}

int Local_new() {
  Mutex_lock(__xcl_mutex);
  int id = Local_allocId();
  Mutex_unlock(__xcl_mutex);
  return id;
}

void* Local_storage() {
  void* obj = osLocalGet(__xcl_key);
  if (!obj) {
    obj = LocalStorage_new();
    LOG_ASSERT_P(obj != NULL);
    osLocalSet(__xcl_key, obj);
  }
  return obj;
}

bool Local_set0(uintptr_t id, LocalPair pair) {
  return LocalStorage_set(Local_storage(), id, pair);
}
LocalPair* Local_get0(uintptr_t id) { return LocalStorage_get(Local_storage(), id); }

bool Local_set(int id, void* attach, void (*destructor)(void*)) {
  return Local_set0(id + LOCAL_ID_MAX_RESERVED, LocalPair_make(attach, destructor));
}
void* Local_get(int id) { return Local_get0(id + LOCAL_ID_MAX_RESERVED)->attach; }
void Local_free(int id) {
  Mutex_lock(__xcl_mutex);
  if (id >= 0 && local_id_vec)
    Vec_push(local_id_vec, &id);
  Mutex_unlock(__xcl_mutex);
}
