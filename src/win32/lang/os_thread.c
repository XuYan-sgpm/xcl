#include <Windows.h>
#include <process.h>
#include <stdbool.h>
#include <xcl/lang/thread.h>
#include <xcl/util/log.h>

static unsigned __stdcall Thread_routine(void* args) {
  run(args);
  _endthreadex(0);
  return 0;
}
uintptr_t Thread_create(struct Runnable* runnable) {
  return _beginthreadex(NULL, 0, Thread_routine, runnable, 0, NULL);
}
bool Thread_join(uintptr_t thread) { return Thread_join2(thread, INFINITE) == 1; }
int Thread_join2(uintptr_t thread, unsigned timeout) {
  DWORD err = WaitForSingleObject((HANDLE)thread, timeout);
  if (err != WAIT_OBJECT_0)
    LOG_P(LOG_ERROR, "os join thread failed:%u, %d, %p", err, GetLastError(), thread);
  return err == WAIT_OBJECT_0 ? 1 : err == WAIT_TIMEOUT ? 0 : -1;
}
bool Thread_detach(uintptr_t thread) { return CloseHandle((void*)thread); }
unsigned long Thread_currentId() { return GetThreadId(GetCurrentThread()); }
uintptr_t Thread_current() { return (uintptr_t)GetCurrentThread(); }
int Thread_alive(uintptr_t thread) {
  DWORD code;
  return !GetExitCodeThread((HANDLE)thread, &code) ? -1 : code == STILL_ACTIVE;
}
