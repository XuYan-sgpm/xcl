//
// Created by xuyan on 2022/7/5.
//

#include <Windows.h>
#include <synchapi.h>
#include "xcl/concurrent/CCond.h"
#include <WinBase.h>

typedef struct {
  CONDITION_VARIABLE conditionVariable;
} CWinCond;

XCL_PUBLIC(void*)
Cond_new() {
  CWinCond* cond = (CWinCond*)malloc(sizeof(CWinCond));
  if (cond) {
    InitializeConditionVariable(&cond->conditionVariable);
  }
  return cond;
}
XCL_PUBLIC(bool)
Cond_delete(void* cond) { return true; }
XCL_PUBLIC(bool)
Cond_wait(void* mutex, void* cond) {
  if (cond) {
    return SleepConditionVariableCS(
        &((CWinCond*)cond)->conditionVariable, mutex, INFINITE);
  }
  return false;
}
XCL_PUBLIC(bool)
Cond_waitFor(void* mutex, void* cond, int32_t millis) {
  if (!cond || !mutex) {
    return false;
  }
  return SleepConditionVariableCS(
      &((CWinCond*)cond)->conditionVariable, mutex, millis);
}
XCL_PUBLIC(bool)
Cond_signal(void* cond) {
  if (cond) {
    WakeConditionVariable(&((CWinCond*)cond)->conditionVariable);
    return true;
  } else {
    return false;
  }
}
XCL_PUBLIC(bool)
Cond_signalAll(void* cond) {
  if (cond) {
    WakeAllConditionVariable(&((CWinCond*)cond)->conditionVariable);
    return true;
  } else {
    return false;
  }
}
