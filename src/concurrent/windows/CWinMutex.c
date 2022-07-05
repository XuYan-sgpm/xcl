//
// Created by xuyan on 2022/7/4.
//


#include <windef.h>
#include <synchapi.h>
#include <mmsystem.h>
#include "concurrent/CMutex.h"
#include "util/system.h"

typedef struct {
  CRITICAL_SECTION criticalSection;
} WinMutex;

void *Mutex_new() {
  WinMutex *mutex = (WinMutex *)malloc(sizeof(WinMutex));
  if (mutex) {
    InitializeCriticalSection(&mutex->criticalSection);
  }
  return mutex;
}
void Mutex_delete(void *mutex) {
  if (mutex) {
    DeleteCriticalSection(&((WinMutex *)mutex)->criticalSection);
  }
}
bool Mutex_lock(void *mutex) {
  if (mutex) {
    EnterCriticalSection(&((WinMutex *)mutex)->criticalSection);
    return true;
  } else {
    return false;
  }
}
void Mutex_unlock(void *mutex) {
  if (mutex) {
    LeaveCriticalSection(&((WinMutex *)mutex)->criticalSection);
  }
}
bool Mutex_tryLock(void *mutex) {
  if (!mutex) {
    return false;
  }
  return TryEnterCriticalSection(&((WinMutex *)mutex)->criticalSection);
}
bool Mutex_tryLock2(void *mutex, int32_t millis) {
  if (!mutex) {
    return false;
  }
  int64_t st = nanos();
  timeBeginPeriod(1);
  int64_t totalWait = 0;
  int64_t nanoTimeout = millis * 1000000;
  bool acquired = false;
  while ((nanoTimeout - totalWait) > 500000) {
    if (TryEnterCriticalSection(&((WinMutex *)mutex)->criticalSection)) {
      acquired = true;
      break;
    }
    Sleep(1);
    totalWait = nanos() - st;
  }
  timeEndPeriod(1);
  if (!acquired) {
    acquired = TryEnterCriticalSection(&((WinMutex *)mutex)->criticalSection);
  }
  return acquired;
}
