//
// Created by 14049 on 2024/7/24.
//

#include <concurrent/condition.h>
#include <concurrent/mutex.h>
#include <internal/xcl.h>
#include <stddef.h>
#include <xcl/concurrent/util/lazy.h>

#define INVALID_POINTER ((void*)~((uintptr_t)0))
#define is_pointer_valid(p) ((p) && (p) != INVALID_POINTER)

#ifdef HAS_ATOMIC
#include <internal/concurrent/atomic.h>

bool Lazy_invoke(LazyFlag* flag, Callable* callable) {
  if (_load8(&flag->value) == 2)
    return true;
  void* mut = __xcl_mutex;
  void* cond = __xcl_cv;
  bool ret = true;
  Mutex_lock(mut);
  while (_load8(&flag->value) == 1)
    Cond_wait(mut, cond);
  if (_load8(&flag->value) == 0) {
    _exchange8(&flag->value, 1);
    Mutex_unlock(mut);
    ret = callable->__vptr->call(callable);
    Mutex_lock(mut);
    _exchange8(&flag->value, ret ? 2 : 0);
    Mutex_unlock(mut);
  } else {
    Mutex_unlock(mut);
  }
  return ret;
}

void* Lazy_get(LazyObject* obj, Factory* factory) {
  void* p = _load_p(&obj->addr);
  if (is_pointer_valid(p))
    return p;
  void* mut = __xcl_mutex;
  void* cond = __xcl_cv;
  Mutex_lock(mut);
  while ((p = _load_p(&obj->addr)) == INVALID_POINTER)
    Cond_wait(mut, cond);
  if ((p = _load_p(&obj->addr)) == NULL) {
    _exchange_p(&obj->addr, INVALID_POINTER);
    Mutex_unlock(mut);
    p = factory->__vptr->create(factory);
    Mutex_lock(mut);
    _exchange_p(&obj->addr, p);
    Mutex_unlock(mut);
  } else {
    Mutex_unlock(mut);
  }
  return p;
}
#else
bool Lazy_invoke(LazyFlag* flag, Callable* callable) {
  void* mut = __xcl_mutex;
  void* cond = __xcl_cv;
  bool ret = true;
  Mutex_lock(mut);
  while (flag->value == 1)
    Cond_wait(mut, cond);
  if (flag->value == 0) {
    flag->value = 1;
    Mutex_unlock(mut);
    ret = callable->__vptr->call(callable);
    Mutex_lock(mut);
    flag->value = ret ? 2 : 0;
    Mutex_unlock(mut);
  } else {
    Mutex_unlock(mut);
  }
  return ret;
}

void* Lazy_get(LazyObject* obj, Factory* factory) {
  void* p = NULL;
  void* mut = __xcl_mutex;
  void* cond = __xcl_cv;
  Mutex_lock(mut);
  while ((p = obj->addr) == INVALID_POINTER)
    Cond_wait(mut, cond);
  if ((p = obj->addr) == NULL) {
    obj->addr = INVALID_POINTER;
    Mutex_unlock(mut);
    p = factory->__vptr->create(factory);
    Mutex_lock(mut);
    obj->addr = p;
    Mutex_unlock(mut);
  } else {
    Mutex_unlock(mut);
  }
  return p;
}
#endif
