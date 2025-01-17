//
// Created by 徐琰 on 2024/4/30.
//
#include <internal/concurrent/atomic.h>
#include <stdatomic.h>

char _load8(volatile char* obj) { return __atomic_load_n(obj, __ATOMIC_ACQUIRE); }
void _store8(volatile char* obj, char val) { __atomic_store_n(obj, val, __ATOMIC_RELEASE); }
char _fetch_add8(volatile char* obj, char delta) {
  return __atomic_fetch_add(obj, delta, __ATOMIC_ACQ_REL);
}
char _fetch_sub8(volatile char* obj, char delta) {
  return __atomic_fetch_sub(obj, delta, __ATOMIC_ACQ_REL);
}
char _exchange8(volatile char* obj, char val) {
  return __atomic_exchange_n(obj, val, __ATOMIC_ACQ_REL);
}
bool _cas8(volatile char* obj, char* expected, char desired) {
  return __atomic_compare_exchange_n(obj, expected, desired, false, __ATOMIC_ACQ_REL,
                                     __ATOMIC_ACQUIRE);
}

short _load16(volatile short* obj) { return __atomic_load_n(obj, __ATOMIC_ACQUIRE); }
void _store16(volatile short* obj, short val) { __atomic_store_n(obj, val, __ATOMIC_RELEASE); }
short _fetch_add16(volatile short* obj, short delta) {
  return __atomic_fetch_add(obj, delta, __ATOMIC_ACQ_REL);
}
short _fetch_sub16(volatile short* obj, short delta) {
  return __atomic_fetch_sub(obj, delta, __ATOMIC_ACQ_REL);
}
short _exchange16(volatile short* obj, short val) {
  return __atomic_exchange_n(obj, val, __ATOMIC_ACQ_REL);
}
bool _cas16(volatile short* obj, short* expected, short desired) {
  return __atomic_compare_exchange_n(obj, expected, desired, false, __ATOMIC_ACQ_REL,
                                     __ATOMIC_ACQUIRE);
}

int _load32(volatile int* obj) { return __atomic_load_n(obj, __ATOMIC_ACQUIRE); }
void _store32(volatile int* obj, int val) { __atomic_store_n(obj, val, __ATOMIC_RELEASE); }
int _fetch_add32(volatile int* obj, int delta) {
  return __atomic_fetch_add(obj, delta, __ATOMIC_ACQ_REL);
}
int _fetch_sub32(volatile int* obj, int delta) {
  return __atomic_fetch_sub(obj, delta, __ATOMIC_ACQ_REL);
}
int _exchange32(volatile int* obj, int val) {
  return __atomic_exchange_n(obj, val, __ATOMIC_ACQ_REL);
}
bool _cas32(volatile int* obj, int* expected, int desired) {
  return __atomic_compare_exchange_n(obj, expected, desired, false, __ATOMIC_ACQ_REL,
                                     __ATOMIC_ACQUIRE);
}

void* _load_p(void* volatile* obj) { return __atomic_load_n(obj, __ATOMIC_ACQUIRE); }
void _store_p(void* volatile* obj, void* ptr) { __atomic_store_n(obj, ptr, __ATOMIC_RELEASE); }
void* _exchange_p(void* volatile* obj, void* ptr) {
  return __atomic_exchange_n(obj, ptr, __ATOMIC_ACQ_REL);
}
bool _cas_p(void* volatile* obj, void** expected, void* desired) {
  return __atomic_compare_exchange_n(obj, expected, desired, false, __ATOMIC_ACQ_REL,
                                     __ATOMIC_ACQUIRE);
}

int64_t _load64(volatile int64_t* obj) { return __atomic_load_n(obj, __ATOMIC_ACQUIRE); }
void _store64(volatile int64_t* obj, int64_t val) { __atomic_store_n(obj, val, __ATOMIC_RELEASE); }
int64_t _fetch_add64(volatile int64_t* obj, int64_t delta) {
  return __atomic_fetch_add(obj, delta, __ATOMIC_ACQ_REL);
}
int64_t _fetch_sub64(volatile int64_t* obj, int64_t delta) {
  return __atomic_fetch_sub(obj, delta, __ATOMIC_ACQ_REL);
}
int64_t _exchange64(volatile int64_t* obj, int64_t val) {
  return __atomic_exchange_n(obj, val, __ATOMIC_ACQ_REL);
}
bool _cas64(volatile int64_t* obj, int64_t* expected, int64_t desired) {
  return __atomic_compare_exchange_n(obj, expected, desired, false, __ATOMIC_ACQ_REL,
                                     __ATOMIC_ACQUIRE);
}