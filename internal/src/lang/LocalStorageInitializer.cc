//
// Created by xuyan on 2022/6/22.
//

#ifdef DYNAMIC_LIB

#include <lang/CLocalStorage.h>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <cassert>

static pthread_key_t __threadLocalStorageKey = -1u;

static void __destroyLocalStorage(void *args) {
  LS_Free(static_cast<CLocalStorage *>(args));
}

namespace {
class __LocalStorageInitImpl {
 public:
  __LocalStorageInitImpl();
  ~__LocalStorageInitImpl();
};
__LocalStorageInitImpl::__LocalStorageInitImpl() {
  auto ret =
      ::pthread_key_create(&__threadLocalStorageKey, __destroyLocalStorage);
  if (ret == 0) {
    assert(__threadLocalStorageKey != -1u);
    auto *localStorage = (CLocalStorage *)malloc(sizeof(CLocalStorage));
    if (localStorage) {
      memset(localStorage, 0, sizeof(CLocalStorage));
      if (::pthread_setspecific(__threadLocalStorageKey, localStorage) == 0) {
        return;
      }
      free(localStorage);
    }
    ::pthread_key_delete(__threadLocalStorageKey);
    __threadLocalStorageKey = -1u;
  } else {
    __threadLocalStorageKey = -1u;
  }
}
__LocalStorageInitImpl::~__LocalStorageInitImpl() {
  if (__threadLocalStorageKey != -1u) {
    ::pthread_key_delete(__threadLocalStorageKey);
    __threadLocalStorageKey = -1u;
  }
}

static __LocalStorageInitImpl __impl;
} // namespace

#ifdef __cplusplus
extern "C" {
#endif

CLocalStorage *__getLocalStorage() {
  if (__threadLocalStorageKey == -1u) {
    return nullptr;
  }
  return static_cast<CLocalStorage *>(
      ::pthread_getspecific(__threadLocalStorageKey));
}

void __setLocalStorage(CLocalStorage *localStorage) {
  if (__threadLocalStorageKey != -1u) {
    ::pthread_setspecific(__threadLocalStorageKey, localStorage);
  }
}

#ifdef __cplusplus
}
#endif

#endif