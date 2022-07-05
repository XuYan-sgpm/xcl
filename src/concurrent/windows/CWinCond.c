//
// Created by xuyan on 2022/7/5.
//

#include <synchapi.h>
#include "concurrent/CCond.h"
#include <concurrent/CMutex.h>
#include <winbase.h>

typedef struct {
  CONDITION_VARIABLE conditionVariable;
} CWinCond;

void *Cond_new() {
  CWinCond *cond = (CWinCond *)malloc(sizeof(CWinCond));
  if (cond) {
    InitializeConditionVariable(&cond->conditionVariable);
  }
  return cond;
}
void Cond_delete(void *cond) {}
bool Cond_wait(void *mutex, void *cond) {
  if (cond) {
    return SleepConditionVariableCS(
        &((CWinCond *)cond)->conditionVariable, Mutex_ctx(mutex), INFINITE);
  }
  return false;
}
bool Cond_waitFor(void *mutex, void *cond, int32_t millis) {
  if (!cond || !mutex) {
    return false;
  }
  return SleepConditionVariableCS(
      &((CWinCond *)cond)->conditionVariable, Mutex_ctx(mutex), millis);
}
void Cond_signal(void *cond) {
  if (cond) {
    WakeConditionVariable(&((CWinCond *)cond)->conditionVariable);
  }
}
void Cond_signalAll(void *cond) {
  if (cond) {
    WakeAllConditionVariable(&((CWinCond *)cond)->conditionVariable);
  }
}
