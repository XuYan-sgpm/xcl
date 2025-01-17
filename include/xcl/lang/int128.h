#pragma once

#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
  struct {
    unsigned long long low;
    long long high;
  };
  long long data[2];
} _i128;
typedef union {
  struct {
    unsigned long long low;
    unsigned long long high;
  };
  unsigned long long data[2];
} _u128;

_i128 _i128_add(_i128 x, _i128 y);
_i128 _i128_sub(_i128 x, _i128 y);
inline always_inline int _i128_cmp(_i128 x, _i128 y) {
  if (x.high != y.high)
    return x.high > y.high ? 1 : -1;
  if (x.low != y.low)
    return x.low > y.low ? 1 : -1;
  return 0;
}
inline always_inline _i128 _i128_val(long long high, unsigned long long low) {
  _i128 val = {low, high};
  return val;
}
inline always_inline _i128 _i128_convert(_u128 val) {
  _i128 ret;
  ret.high = *(long long*)&val.high;
  ret.low = val.low;
  return ret;
}
inline always_inline int64_t _i128_high(_i128 val) { return val.high; }
inline always_inline uint64_t _i128_low(_i128 val) { return val.low; }
_u128 _u128_add(_u128 x, _u128 y);
_u128 _u128_sub(_u128 x, _u128 y);
inline always_inline int _u128_cmp(_u128 x, _u128 y) {
  if (x.high != y.high)
    return x.high > y.high ? 1 : -1;
  if (x.low != y.low)
    return x.low > y.low ? 1 : -1;
  return 0;
}
inline always_inline _u128 _u128_val(unsigned long long high, unsigned long long low) {
  _u128 val = {low, high};
  return val;
}
inline always_inline _u128 _u128_convert(_i128 val) {
  _u128 ret;
  ret.high = *(unsigned long long*)&val.high;
  ret.low = val.low;
  return ret;
}
inline always_inline uint64_t _u128_high(_u128 val) { return val.high; }
inline always_inline uint64_t _u128_low(_u128 val) { return val.low; }

#ifdef __cplusplus
}
#endif
