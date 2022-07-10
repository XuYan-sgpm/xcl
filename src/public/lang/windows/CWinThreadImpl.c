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
                ThreadProc proc,
                void* usr,
                ThreadHandle* handle,
                unsigned int* tid) {
  ThreadHandle h = (ThreadHandle)_beginthreadex(
      NULL, 0, proc, usr, suspend ? CREATE_SUSPENDED : 0, tid);
  *handle = h;
  return h != NULL;
}
void
__Thread_resume(CThread* thread) {
  __Thread_setState(thread, ALIVE);
  ResumeThread(__Thread_handle(thread));
}
void
__Thread_onFinish(CThread* thread, unsigned int retVal) {
  _endthreadex(retVal);
}
unsigned
__Thread_currentId() {
  return GetCurrentThreadId();
}
ThreadHandle
__Thread_currentHandle(unsigned int tid) {
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
