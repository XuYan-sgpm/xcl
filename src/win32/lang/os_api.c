//
// Created by xuyan on 2022/7/4.
//

#include <Windows.h>
#include <internal/concurrent/atomic.h>
#include <malloc.h>
#include <profileapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <sysinfoapi.h>
#include <time.h>
#include <xcl/lang/system.h>
#include <xcl/util/log.h>

int64_t currentMillis() { return GetTickCount(); }

int _gettimeofday(struct _timeval* tv, void* zone) {
  // This magic number is the number of 100 nanosecond intervals since January
  // 1, 1601 (UTC) until 00:00:00 January 1, 1970
  static const uint64_t epoch = ((uint64_t)116444736000000000ULL);
  FILETIME file_time;
  GetSystemTimeAsFileTime(&file_time);
  uint64_t time = ((uint64_t)file_time.dwLowDateTime) + ((uint64_t)file_time.dwHighDateTime << 32);
  uint64_t us = (uint64_t)((time - epoch) / 10ull);
  tv->tv_sec = us / 1000000ull;
  tv->tv_usec = us % 1000000ull;
  return 0;
}

int64_t nanos() {
  static _aligned(8) volatile int64_t nano_freq = 0;
  int64_t val = _InterlockedCompareExchange64(&nano_freq, 0, 0);
  if (val == 0) {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    val = freq.QuadPart;
    _InterlockedCompareExchange64(&nano_freq, val, 0);
  }
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);
  return (int64_t)((double)current.QuadPart * ((double)1000000000ull / (double)val));
}

void msleep(int32_t timeout) { Sleep(timeout); }

unsigned error(int op, ...) {
  if (op == 1) {
    va_list ap;
    va_start(ap, op);
    SetLastError(va_arg(ap, unsigned));
    va_end(ap);
  }
  return GetLastError();
}

void* Dl_open(const char* str) {
  LOG_P(LOG_DEBUG, "open dl -> %s", str);
  return LoadLibrary(str);
}

void* Dl_sym(void* handle, const char* symbol) {
  return handle ? GetProcAddress(handle, symbol) : NULL;
}

void Dl_close(void* handle) {
  if (handle)
    FreeLibrary(handle);
}
