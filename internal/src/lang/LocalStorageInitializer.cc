//
// Created by xuyan on 2022/6/22.
//

namespace xcl {
#include <lang/CLocalStorage.h>

#ifdef DYNAMIC

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
    //    auto *localStorage = (CLocalStorage *)malloc(sizeof(CLocalStorage));
    //    if (localStorage) {
    //      memset(localStorage, 0, sizeof(CLocalStorage));
    //      if (::pthread_setspecific(__threadLocalStorageKey, localStorage) ==
    //      0) {
    //        return;
    //      }
    //      free(localStorage);
    //    }
    //    ::pthread_key_delete(__threadLocalStorageKey);
    //    __threadLocalStorageKey = -1u;
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
} // namespace

static __LocalStorageInitImpl __impl;

#elif STATIC

#include <lang/platform.h>

#if GNUC || CLANG
#define THREAD_LOCAL __thread
#elif MSVC
#define THREAD_LOCAL __declspec(thread)
#else
#error "unsupported compiler"
#endif

static THREAD_LOCAL CLocalStorage *__threadLocalStorage = nullptr;

class __LocalStorageRegImpl {

};

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DYNAMIC

CLocalStorage *__TL_getLocalStorage() {
  if (__threadLocalStorageKey == -1u) {
    return nullptr;
  }
  return static_cast<CLocalStorage *>(
      ::pthread_getspecific(__threadLocalStorageKey));
}

bool __TL_setLocalStorage(CLocalStorage *localStorage) {
  if (__threadLocalStorageKey == -1u) {
    return false;
  } else {
    auto ret = ::pthread_setspecific(__threadLocalStorageKey, localStorage);
    return ret == 0;
  }
}

#elif STATIC

CLocalStorage *__TL_getLocalStorage() { return __threadLocalStorage; }
bool __TL_setLocalStorage(CLocalStorage *localStorage) {
  __threadLocalStorage = localStorage;
  return true;
}

#endif

#ifdef __cplusplus
}
#endif
}