#include <Windows.h>
#include <internal/concurrent/atomic.h>
#include <intrin0.inl.h>
#include <winnt.h>

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
#error xcl is not support for win32 arm platform
#endif

#define MEMORY_FENCE _ReadWriteBarrier()

char _load8(volatile char* obj) {
  char val = *obj;
  MEMORY_FENCE;
  return val;
}
void _store8(volatile char* obj, char val) {
  MEMORY_FENCE;
  *obj = val;
}
char _fetch_add8(volatile char* obj, char delta) { return _InterlockedExchangeAdd8(obj, delta); }
char _fetch_sub8(volatile char* obj, char delta) { return _InterlockedExchangeAdd8(obj, -delta); }
char _exchange8(volatile char* obj, char val) { return _InterlockedExchange8(obj, val); }
bool _cas8(volatile char* obj, char* expected, char desired) {
  char val = _InterlockedCompareExchange8(obj, desired, *expected);
  return val == *expected ? true : (*expected = val, false);
}

short _load16(volatile short* obj) {
  short val = *obj;
  MEMORY_FENCE;
  return val;
}
void _store16(volatile short* obj, short val) {
  MEMORY_FENCE;
  *obj = val;
}
short _fetch_add16(volatile short* obj, short delta) {
  return _InterlockedExchangeAdd16(obj, delta);
}
short _fetch_sub16(volatile short* obj, short delta) {
  return _InterlockedExchangeAdd16(obj, -delta);
}
short _exchange16(volatile short* obj, short val) { return _InterlockedExchange16(obj, val); }
bool _cas16(volatile short* obj, short* expected, short desired) {
  short val = _InterlockedCompareExchange16(obj, desired, *expected);
  return val == *expected ? true : (*expected = val, false);
}

int _load32(volatile int* obj) {
  int val = *obj;
  MEMORY_FENCE;
  return val;
}
void _store32(volatile int* obj, int val) {
  MEMORY_FENCE;
  *obj = val;
}
int _fetch_add32(volatile int* obj, int delta) {
  return _InterlockedExchangeAdd((volatile long*)obj, delta);
}
int _fetch_sub32(volatile int* obj, int delta) {
  return _InterlockedExchangeAdd((volatile long*)obj, -delta);
}
int _exchange32(volatile int* obj, int val) {
  return _InterlockedExchange((volatile long*)obj, (long)val);
}
bool _cas32(volatile int* obj, int* expected, int desired) {
  long val = _InterlockedCompareExchange((volatile long*)obj, desired, *expected);
  return val == *expected ? true : (*expected = val, false);
}

void* _load_p(void* volatile* obj) {
  void* ptr = *obj;
  MEMORY_FENCE;
  return ptr;
}
void _store_p(void* volatile* obj, void* ptr) {
  MEMORY_FENCE;
  *obj = ptr;
}
void* _exchange_p(void* volatile* obj, void* ptr) { return InterlockedExchangePointer(obj, ptr); }
bool _cas_p(void* volatile* obj, void** expected, void* desired) {
  void* ptr = InterlockedCompareExchangePointer(obj, desired, *expected);
  return ptr == *expected ? true : (*expected = ptr, false);
}

#ifdef _WIN64
int64_t _load64(volatile int64_t* obj) {
  int64_t val = *obj;
  MEMORY_FENCE;
  return val;
}
void _store64(volatile int64_t* obj, int64_t val) {
  MEMORY_FENCE;
  *obj = val;
}
#else
int64_t _load64(volatile int64_t* obj) { return InterlockedOr64(obj, 0); }
void _store64(volatile int64_t* obj, int64_t val) { InterlockedExchange64(obj, val); }
#endif
int64_t _fetch_add64(volatile int64_t* obj, int64_t delta) {
  return InterlockedExchangeAdd64(obj, delta);
}
int64_t _fetch_sub64(volatile int64_t* obj, int64_t delta) {
  return InterlockedExchangeAdd64(obj, -delta);
}
int64_t _exchange64(volatile int64_t* obj, int64_t desired) {
  return InterlockedExchange64(obj, desired);
}
bool _cas64(volatile int64_t* obj, int64_t* expected, int64_t desired) {
  int64_t val = _InterlockedCompareExchange64(obj, desired, *expected);
  return val == *expected ? true : (*expected = val, false);
}
