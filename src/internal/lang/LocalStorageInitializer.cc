//
// Created by xuyan on 2022/6/22.
//

#include <cstdlib>
#include <cstring>
#include <cassert>
#include "xcl/lang/CLocalStorage.h"

#ifdef DYNAMIC

#elif STATIC

#  include <lang/XclDef.h>
//#include <mutex>
#  include <concurrent/Lock.h>
#  include <util/CSingleList.h>

#  if GNUC || CLANG
#    define THREAD_LOCAL __thread
#  elif MSVC
#    define THREAD_LOCAL __declspec(thread)
#  else
#    error "unsupported compiler"
#  endif

static THREAD_LOCAL CLocalStorage* __threadLocalStorage = NULL;

namespace {
class LocalStorageRegImpl {
 public:
  void
  regLocalStorage(CLocalStorage* localStorage);

 public:
  LocalStorageRegImpl();
  ~LocalStorageRegImpl();

 private:
  xcl::Lock* listLock_;
  CSingleList storageList_;
};
LocalStorageRegImpl::LocalStorageRegImpl() : listLock_(xcl::Lock::NewLock()) {
  storageList_ = SingleList_new();
}
LocalStorageRegImpl::~LocalStorageRegImpl() {
  delete listLock_;
  listLock_ = NULL;
  CSingleNode* node = NULL;
  for (;;) {
    node = SingList_popFront(&storageList_);
    if (!node) {
      break;
    }
    CLocalStorage* localStorage = (CLocalStorage*)*(intptr_t*)node->data;
    LocalStorage_free(localStorage);
    free(node);
  }
}
void
LocalStorageRegImpl::regLocalStorage(CLocalStorage* localStorage) {
  CSingleNode* node = (CSingleNode*)malloc(sizeof(CSingleNode) + sizeof(void*));
  assert(node);
  memset(node, 0, sizeof(*node) + sizeof(void*));
  *(intptr_t*)(node->data) = (intptr_t)localStorage;
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

CLocalStorage*
__Local_getLocalStorage() {
  return NULL;
}

bool
__Local_setLocalStorage(CLocalStorage* localStorage) {
  return false;
}

#elif STATIC

CLocalStorage*
__Local_getLocalStorage() {
  return __threadLocalStorage;
}
bool
__Local_setLocalStorage(CLocalStorage* localStorage) {
  __threadLocalStorage = localStorage;
  __localStorageRegImpl.regLocalStorage(localStorage);
  return true;
}

#endif

#ifdef __cplusplus
}
#endif