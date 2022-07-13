//
// Created by xuyan on 2022/7/4.
//

#include "xcl/lang/system.h"
#include <Windows.h>
#include <sysinfoapi.h>
#include <profileapi.h>

static int64_t __NANO_FREQ_PER_SEC = -1;

static void
__SYS_initNanoFreq() {
  LARGE_INTEGER freq;
  if (QueryPerformanceFrequency(&freq)) {
    __NANO_FREQ_PER_SEC = freq.QuadPart;
  }
}

XCL_EXPORT(int64_t)
currentMillis() { return GetTickCount(); }
XCL_EXPORT(int64_t)
nanos() {
  if (__NANO_FREQ_PER_SEC == -1) {
    __SYS_initNanoFreq();
  }
  if (__NANO_FREQ_PER_SEC > 0) {
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    double tmp = ((double)NANOS_PER_SEC / (double)__NANO_FREQ_PER_SEC);
    return (int64_t)((double)current.QuadPart * tmp);
  } else {
    return -1;
  }
}
XCL_EXPORT(void)
sleepMillis(int32_t timeout) {
  timeBeginPeriod(1);
  Sleep(timeout);
  timeEndPeriod(1);
}
XCL_EXPORT(unsigned)
error() { return GetLastError(); }
XCL_EXPORT(void)
setErr(unsigned errorCode) { SetLastError(errorCode); }
