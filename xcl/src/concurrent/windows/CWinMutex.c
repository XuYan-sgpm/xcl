//
// Created by xuyan on 2022/7/4.
//

#include <windows.h>
#include "concurrent/CMutex.h"
#include "util/system.h"

typedef struct {
  CRITICAL_SECTION criticalSection;
} CWinMutex;

XCL_PUBLIC void* XCL_API
Mutex_new() {
  CWinMutex* mutex = (CWinMutex*)malloc(sizeof(CWinMutex));
  if (mutex) {
    InitializeCriticalSection(&mutex->criticalSection);
  }
  return mutex;
}
XCL_PUBLIC void XCL_API
Mutex_delete(void* mutex) {
  if (mutex) {
    DeleteCriticalSection(&((CWinMutex*)mutex)->criticalSection);
  }
}
XCL_PUBLIC bool XCL_API
Mutex_lock(void* mutex) {
  if (mutex) {
    EnterCriticalSection(&((CWinMutex*)mutex)->criticalSection);
    return true;
  } else {
    return false;
  }
}
XCL_PUBLIC void XCL_API
Mutex_unlock(void* mutex) {
  if (mutex) {
    LeaveCriticalSection(&((CWinMutex*)mutex)->criticalSection);
  }
}
XCL_PUBLIC bool XCL_API
Mutex_tryLock(void* mutex) {
  if (!mutex) {
    return false;
  }
  return TryEnterCriticalSection(&((CWinMutex*)mutex)->criticalSection);
}
XCL_PUBLIC bool XCL_API
Mutex_tryLock2(void* mutex, int32_t millis) {
  if (!mutex) {
    return false;
  }
  int64_t st = nanos();
  timeBeginPeriod(1);
  int64_t totalWait = 0;
  int64_t nanoTimeout = millis * 1000000;
  bool acquired = false;
  while ((nanoTimeout - totalWait) > 500000) {
    if (TryEnterCriticalSection(&((CWinMutex*)mutex)->criticalSection)) {
      acquired = true;
      break;
    }
    Sleep(1);
    totalWait = nanos() - st;
  }
  timeEndPeriod(1);
  if (!acquired) {
    acquired = TryEnterCriticalSection(&((CWinMutex*)mutex)->criticalSection);
  }
  return acquired;
}
