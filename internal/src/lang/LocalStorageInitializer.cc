//
// Created by xuyan on 2022/6/22.
//

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <pthread.h>
#include <lang/CLocalStorage.h>

#ifdef DYNAMIC

static pthread_key_t __threadLocalStorageKey = -1u;

static void __destroyLocalStorage(void *args) {
  LS_Free(static_cast<CLocalStorage *>(args));
}

namespace {
class __LocalStorageKeyInitImpl {
 public:
  __LocalStorageKeyInitImpl();
  ~__LocalStorageKeyInitImpl();
};
__LocalStorageKeyInitImpl::__LocalStorageKeyInitImpl() {
  auto ret =
      ::pthread_key_create(&__threadLocalStorageKey, __destroyLocalStorage);
  if (ret == 0) {
    assert(__threadLocalStorageKey != -1u);
  } else {
    __threadLocalStorageKey = -1u;
  }
}
__LocalStorageKeyInitImpl::~__LocalStorageKeyInitImpl() {
  if (__threadLocalStorageKey != -1u) {
    ::pthread_key_delete(__threadLocalStorageKey);
    __threadLocalStorageKey = -1u;
  }
}
static __LocalStorageKeyInitImpl __impl;
} // namespace

#elif STATIC

#include <lang/platform.h>
//#include <mutex>
#include <concurrent/Lock.h>
#include <util/CSingleList.h>

#if GNUC || CLANG
#define THREAD_LOCAL __thread
#elif MSVC
#define THREAD_LOCAL __declspec(thread)
#else
#error "unsupported compiler"
#endif

static THREAD_LOCAL CLocalStorage *__threadLocalStorage = NULL;

namespace {
class LocalStorageRegImpl {
 public:
  void regLocalStorage(CLocalStorage *localStorage);

 public:
  LocalStorageRegImpl();
  ~LocalStorageRegImpl();

 private:
  xcl::Lock *listLock_;
  CSingleList storageList_;
};
LocalStorageRegImpl::LocalStorageRegImpl()
    : listLock_(xcl::Lock::NewLock()) {
  storageList_ = SingleList_new();
}
LocalStorageRegImpl::~LocalStorageRegImpl() {
  delete listLock_;
  listLock_ = NULL;
  CSingleNode *node = NULL;
  for (;;) {
    node = SingList_popFront(&storageList_);
    if (!node) {
      break;
    }
    CLocalStorage *localStorage = (CLocalStorage *)*(intptr_t *)node->data;
    LS_Free(localStorage);
    free(node);
  }
}
void LocalStorageRegImpl::regLocalStorage(CLocalStorage *localStorage) {
  CSingleNode *node =
      (CSingleNode *)malloc(sizeof(CSingleNode) + sizeof(void *));
  assert(node);
  memset(node, 0, sizeof(*node) + sizeof(void *));
  *(intptr_t *)(node->data) = (intptr_t)localStorage;
  listLock_->lock();
  SingleList_pushFront(&storageList_, node);
  listLock_->unlock();
}

static LocalStorageRegImpl __localStorageRegImpl;
} // namespace

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
  __localStorageRegImpl.regLocalStorage(localStorage);
  return true;
}

#endif

#ifdef __cplusplus
}
#endif