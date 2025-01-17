//
// Created by 徐琰 on 2024/2/18.
//

#include <internal/concurrent/atomic.h>
#include <xcl/lang/define.h>

static void atomic_test_8() {
  int8_t ret, val = 0;
  _aligned(1) volatile uint8_t obj = {0};
  ret = _load8(&obj);
  _store8(&obj, val);
  ret = _exchange8(&obj, val);
  ret = _fetch_add8(&obj, val);
  _cas8(&obj, &ret, val);
}

static void atomic_test_16() {
  int16_t ret, val = 0;
  _aligned(2) volatile uint16_t obj = {0};
  ret = _load16(&obj);
  _store16(&obj, val);
  ret = _exchange16(&obj, val);
  ret = _fetch_add16(&obj, val);
  _cas16(&obj, &ret, val);
}

static void atomic_test_32() {
  int32_t ret, val = 0;
  _aligned(4) volatile uint32_t obj = {0};
  ret = _load32(&obj);
  _store32(&obj, val);
  ret = _exchange32(&obj, val);
  ret = _fetch_add32(&obj, val);
  _cas32(&obj, &ret, val);
}

static void atomic_test_64() {
  int64_t ret, val = 0;
  _aligned(8) volatile uint64_t obj = {0};
  ret = _load64(&obj);
  _store64(&obj, val);
  ret = _exchange64(&obj, val);
  ret = _fetch_add64(&obj, val);
  _cas64(&obj, &ret, val);
}

int main() {
  atomic_test_8();
  atomic_test_16();
  atomic_test_32();
  atomic_test_64();
  return 0;
}
