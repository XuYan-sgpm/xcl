//
// Created by xuyan on 2022/7/6.
//

#include <xcl/lang/CBaseThreadImpl.h>
#include <windows.h>
#include <synchapi.h>
#include <process.h>

void
__Thread_beforeCreate(CThread* thread) {}
void
__Thread_afterCreate(CThread* thread) {}
void
__Thread_wait(CThread* thread) {
  WaitForSingleObject(__Thread_handle(thread), INFINITE);
}
bool
__Thread_waitTimeout(CThread* thread, int32_t timeout) {
  DWORD ret = WaitForSingleObject(__Thread_handle(thread), timeout);
  return ret == WAIT_OBJECT_0;
}
bool
__Thread_create(bool suspend,
                __ThreadRunProc run,
                void* usr,
                ThreadHandle* handle) {
  ThreadHandle h = (ThreadHandle)_beginthreadex(
      NULL, 0, run, usr, suspend ? CREATE_SUSPENDED : 0, NULL);
  *handle = h;
  return h != NULL;
}
void
__Thread_resume(CThread* thread) {
  __Thread_setState(thread, ALIVE);
  ResumeThread(__Thread_handle(thread));
}
void
__Thread_onStart(CThread* thread) {}
void
__Thread_onFinish(CThread* thread, __ThreadRunReturnType retVal) {
  _endthreadex(retVal);
}
unsigned
__Thread_currentId() {
  return GetCurrentThreadId();
}
ThreadHandle
__Thread_currentHandle(CThread* thread, unsigned tid) {
  return OpenThread(SYNCHRONIZE, false, tid);
}
void
__Thread_finalize(CThread* thread) {
  CloseHandle(__Thread_handle(thread));
}
void
__Thread_detach(CThread* thread) {
  CloseHandle(__Thread_handle(thread));
  __Thread_setState(thread, DETACHED);
}

#ifdef DYNAMIC
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
__initKey() {
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