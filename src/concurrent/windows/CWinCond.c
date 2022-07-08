//
// Created by xuyan on 2022/7/5.
//

#include <windows.h>
#include <synchapi.h>
#include "concurrent/CCond.h"
#include <winbase.h>

typedef struct {
  CONDITION_VARIABLE conditionVariable;
} CWinCond;

XCL_PUBLIC void* XCL_API
Cond_new() {
  CWinCond* cond = (CWinCond*)malloc(sizeof(CWinCond));
  if (cond) {
    InitializeConditionVariable(&cond->conditionVariable);
  }
  return cond;
}
XCL_PUBLIC void XCL_API
Cond_delete(void* cond) {}
XCL_PUBLIC bool XCL_API
Cond_wait(void* mutex, void* cond) {
  if (cond) {
    return SleepConditionVariableCS(
        &((CWinCond*)cond)->conditionVariable, mutex, INFINITE);
  }
  return false;
}
XCL_PUBLIC bool XCL_API
Cond_waitFor(void* mutex, void* cond, int32_t millis) {
  if (!cond || !mutex) {
    return false;
  }
  return SleepConditionVariableCS(
      &((CWinCond*)cond)->conditionVariable, mutex, millis);
}
XCL_PUBLIC void XCL_API
Cond_signal(void* cond) {
  if (cond) {
    WakeConditionVariable(&((CWinCond*)cond)->conditionVariable);
  }
}
XCL_PUBLIC void XCL_API
Cond_signalAll(void* cond) {
  if (cond) {
    WakeAllConditionVariable(&((CWinCond*)cond)->conditionVariable);
  }
}
