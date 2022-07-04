//
// Created by xuyan on 2022/7/4.
//

#include "lang/platform.h"

#if WINDOWS

#include <windef.h>
#include <synchapi.h>
#include <sysinfoapi.h>
#include <mmsystem.h>
#include "concurrent/CMutex.h"

#include <stdio.h>

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
  DWORD st = GetTickCount();
  timeBeginPeriod(1);
  DWORD accuracy = 1;
  DWORD totalWait = 0;
  bool acquired = false;
  int32_t call = 0, threshold = 3;
  while (totalWait < millis) {
    if (TryEnterCriticalSection(&((WinMutex *)mutex)->criticalSection)) {
      acquired = true;
      break;
    }
    ++call;
    if (call >= threshold) {
      totalWait = GetTickCount() - st;
      call = 0;
    }
    if (totalWait >= millis) {
      break;
    }
    DWORD beginSleep = GetTickCount();
    Sleep(accuracy);
    totalWait += GetTickCount() - beginSleep;
  }
  timeEndPeriod(1);
  return acquired;
}

#endif
