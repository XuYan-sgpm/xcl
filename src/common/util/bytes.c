#include <stdlib.h>
#include <string.h>
#include <xcl/util/bytes.h>

int byte2hex(const unsigned char* src, int32_t len, char* dst) {
  if (len <= 0)
    return 0;
  const static char* __hex = "0123456789abcdef";
  int ret = 0;
  for (int i = 0; i < len; i++) {
    unsigned char high = src[i] >> 4;
    unsigned char low = src[i] & 0xf;
    dst[ret++] = __hex[high];
    dst[ret++] = __hex[low];
  }
  return ret;
}

static int __hex_to_num(char ch) {
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  abort();
}

int hex2byte(const char* src, int32_t len, unsigned char* dst) {
  if (len <= 0)
    return 0;
  len &= ~1;
  int ret = 0;
  for (int i = 0; i < len; i += 2) {
    unsigned char val = (__hex_to_num(src[i]) << 4) | __hex_to_num(src[i + 1]);
    dst[ret++] = val;
  }
  return ret;
}

uint8_t Bit8_set(uint8_t val, uint8_t idx, bool on) {
  idx &= 7;
  return on ? val | (1 << idx) : val & ~(1 << idx);
}

uint16_t Bit16_set(uint16_t val, uint8_t idx, bool on) {
  idx &= 0xf;
  return on ? val | (1 << idx) : val & ~(1 << idx);
}

uint32_t Bit32_set(uint32_t val, uint8_t idx, bool on) {
  idx &= 0x1f;
  return on ? val | (1 << idx) : val & ~(1 << idx);
}

uint64_t Bit64_set(uint64_t val, uint8_t idx, bool on) {
  idx &= 0x3f;
  return on ? val | (1 << idx) : val & ~(1 << idx);
}

bool Bit8_isSet(uint8_t val, uint8_t idx) { return val & (1 << (idx & 7)); }

bool Bit16_isSet(uint16_t val, uint8_t idx) { return val & (1 << (idx & 0xf)); }

bool Bit32_isSet(uint32_t val, uint8_t idx) { return val & (1 << (idx & 0x1f)); }

bool Bit64_isSet(uint64_t val, uint8_t idx) { return val & (1 << (idx & 0x3f)); }

static bool __isBig() {
  union {
    short val;
    char data[2];
  } unit;
  unit.val = 0x0001;
  return unit.data[1] == 1;
}

static void __toByte(const void* src, int len, void* buf, bool big) {
  if (__isBig() == big) {
    memcpy(buf, src, len);
    return;
  }
  for (int i = 0; i < len; i++) {
    ((unsigned char*)buf)[len - 1 - i] = ((const unsigned char*)src)[i];
  }
}

void int2byte(int val, void* buf, bool big) { __toByte(&val, sizeof(val), buf, big); }

int byte2int(const void* buf, bool big) {
  int ret;
  __toByte(buf, sizeof(int), &ret, big);
  return ret;
}

void short2byte(short val, void* buf, bool big) { __toByte(&val, sizeof(val), buf, big); }

short byte2short(const void* buf, bool big) {
  short ret;
  __toByte(buf, sizeof(ret), &ret, big);
  return ret;
}

void float2byte(float val, void* buf, bool big) { __toByte(&val, sizeof(val), buf, big); }

float byte2float(const void* buf, bool big) {
  float ret;
  __toByte(buf, sizeof(ret), &ret, big);
  return ret;
}

void long2byte(int64_t val, void* buf, bool big) { __toByte(&val, sizeof(val), buf, big); }

int64_t byte2long(const void* buf, bool big) {
  int64_t ret;
  __toByte(buf, sizeof(ret), &ret, big);
  return ret;
}

void double2byte(double val, void* buf, bool big) { __toByte(&val, sizeof(val), buf, big); }

double byte2double(const void* buf, bool big) {
  double ret;
  __toByte(buf, sizeof(ret), &ret, big);
  return ret;
}
