#include <xcl/lang/int128.h>

_i128 _i128_add(_i128 x, _i128 y) {
  _i128 ret = {0};
  long long step = 0;
  if (x.low > (~0ull) - y.low) {
    ret.low = x.low - ((~0ull) - y.low) - 1;
    ++step;
  } else {
    ret.low = x.low + y.low;
  }
  ret.high = x.high + y.high + step;
  return ret;
}
_i128 _i128_sub(_i128 x, _i128 y) {
  long long sub = 0;
  _i128 ret;
  if (x.low >= y.low) {
    ret.low = x.low - y.low;
  } else {
    ret.low = x.low + ((~0ull) - y.low) + 1;
    ++sub;
  }
  ret.high = x.high - sub - y.high;
  return ret;
}
_u128 _u128_add(_u128 x, _u128 y) {
  _u128 ret;
  int step = 0;
  if (x.low > (~0ull) - y.low) {
    ret.low = x.low - ((~0ull) - y.low) - 1;
    ++step;
  } else {
    ret.low = x.low + y.low;
  }
  ret.high = x.high + y.high + step;
  return ret;
}
_u128 _u128_sub(_u128 x, _u128 y) {
  int sub = 0;
  _u128 ret;
  if (x.low >= y.low) {
    ret.low = x.low - y.low;
  } else {
    ret.low = x.low + ((~0ull) - y.low) + 1;
    ++sub;
  }
  ret.high = x.high - sub - y.high;
  return ret;
}
