//
// Created by xuyan on 2022/7/9.
//

#include <xcl/lang/XclDef.h>
#include <xcl/lang/CLocalStorage.h>

#ifdef STATIC
#  ifdef _MSC_VER
#    define THREAD_LOCAL __declspec(thread)
#  else
#    define THREAD_LOCAL __thread;
#  endif

static THREAD_LOCAL CLocalStorage* __ThreadLocal_storage = NULL;

CLocalStorage*
__ThreadLocal_getLocalStorage() {
  return __ThreadLocal_storage;
}
bool
__ThreadLocal_setLocalStorage(CLocalStorage* localStorage) {
  __ThreadLocal_storage = localStorage;
  return true;
}
#else
#  include <windows.h>
#  include <processthreadsapi.h>
#  include "xcl/lang/CBaseThreadImpl.h"

static DWORD __localStorageKey = TLS_OUT_OF_INDEXES;

void
__LocalInit_allocTls() {
  DWORD idx = TlsAlloc();
  if (idx != TLS_OUT_OF_INDEXES) {
    __localStorageKey = idx;
  }
}

#  if !MSVC
__attribute__((constructor)) static void
__LocalInit_initKey() {
  __LocalInit_allocTls();
}
#  endif

CLocalStorage*
__Thread_getLocalStorage() {
  return (CLocalStorage*)TlsGetValue(__localStorageKey);
}
bool
__Thread_setLocalStorage(CLocalStorage* localStorage) {
  return TlsSetValue(__localStorageKey, localStorage);
}

#endif
