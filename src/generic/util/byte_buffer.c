//
// Created by 14049 on 2023/12/19.
//

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/util/byte_buffer.h>
#include <xcl/util/log.h>

typedef struct {
  unsigned external : 1;
  unsigned endian : 1;
} ByteBufferInfo;

static inline always_inline ByteBufferInfo* __ByteBuffer_info(ByteBuffer* bb) {
  return (ByteBufferInfo*)&bb->info;
}
static inline always_inline bool __ByteBuffer_checkRegion(ByteBuffer* bb, unsigned offset,
                                                          unsigned len) {
  return offset + len <= bb->limit;
}

static void __ByteBuffer_init(ByteBuffer* bb, bool external, void* buf, unsigned cap,
                              unsigned limit, ByteEndian endian) {
  memset(bb, 0, sizeof(*bb));
  if (buf) {
    bb->cap = cap;
    bb->ptr = buf;
    bb->limit = limit;
  }
  __ByteBuffer_info(bb)->external = external;
  __ByteBuffer_info(bb)->endian = endian;
}

ByteEndian localEndian() {
  const static union {
    char data[2];
    short value;
  } u = {.value = 1};
  return u.data[0] == 1;
}

ByteBuffer ByteBuffer_alloc(unsigned cap, ByteEndian endian) {
  ByteBuffer bb;
  __ByteBuffer_init(&bb, false, malloc(cap), cap, cap, endian);
  return bb;
}

ByteBuffer ByteBuffer_wrap(void* buf, unsigned len, ByteEndian endian) {
  ByteBuffer bb;
  __ByteBuffer_init(&bb, true, buf, len, len, endian);
  return bb;
}

ByteBuffer ByteBuffer_slice(ByteBuffer* bb, unsigned offset, unsigned len) {
  LOG_ASSERT_P(offset + len <= bb->cap);
  ByteBuffer slice;
  __ByteBuffer_init(&slice, true, bb->ptr + offset, len, len, __ByteBuffer_info(bb)->endian);
  return slice;
}

void ByteBuffer_destroy(ByteBuffer* bb) {
  if (!__ByteBuffer_info(bb)->external)
    free(bb->ptr);
  memset(bb, 0, sizeof(*bb));
}

bool ByteBuffer_put(ByteBuffer* bb, const void* buf, unsigned len) {
  if (!__ByteBuffer_checkRegion(bb, bb->cursor, len))
    return false;
  memcpy(bb->ptr + bb->cursor, buf, len);
  bb->cursor += len;
  return true;
}

bool ByteBuffer_put8(ByteBuffer* bb, unsigned char val) {
  if (!__ByteBuffer_checkRegion(bb, bb->cursor, 1))
    return false;
  *(unsigned char*)(bb->ptr + (bb->cursor++)) = val;
  return true;
}

bool ByteBuffer_put16(ByteBuffer* bb, unsigned short val) {
  if (!__ByteBuffer_checkRegion(bb, bb->cursor, 2))
    return false;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    *(unsigned short*)&bb->ptr[bb->cursor] = val;
    bb->cursor += 2;
  } else {
    bb->ptr[bb->cursor++] = val >> 8;
    bb->ptr[bb->cursor++] = val & 0xff;
  }
  return true;
}

bool ByteBuffer_put32(ByteBuffer* bb, unsigned val) {
  if (!__ByteBuffer_checkRegion(bb, bb->cursor, 4))
    return false;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    *(unsigned*)&bb->ptr[bb->cursor] = val;
    bb->cursor += 4;
  } else {
    bb->ptr[bb->cursor++] = val >> 24;
    bb->ptr[bb->cursor++] = (val >> 16) & 0xff;
    bb->ptr[bb->cursor++] = (val >> 8) & 0xff;
    bb->ptr[bb->cursor++] = val & 0xff;
  }
  return true;
}

bool ByteBuffer_put64(ByteBuffer* bb, uint64_t val) {
  if (!__ByteBuffer_checkRegion(bb, bb->cursor, 8))
    return false;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    *(uint64_t*)&bb->ptr[bb->cursor] = val;
    bb->cursor += 8;
  } else {
    int steps = 8;
    while (steps-- > 0)
      bb->ptr[bb->cursor++] = ((char*)&val)[steps];
  }
  return true;
}

bool ByteBuffer_set8(ByteBuffer* bb, unsigned offset, unsigned char val) {
  if (!__ByteBuffer_checkRegion(bb, offset, 1))
    return false;
  *(unsigned char*)(bb->ptr + offset) = val;
  return true;
}

bool ByteBuffer_set16(ByteBuffer* bb, unsigned offset, unsigned short val) {
  if (!__ByteBuffer_checkRegion(bb, offset, 2))
    return false;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    *(unsigned short*)&bb->ptr[offset] = val;
  } else {
    bb->ptr[offset] = (val >> 8);
    bb->ptr[offset + 1] = val & 0xff;
  }
  return true;
}

bool ByteBuffer_set32(ByteBuffer* bb, unsigned offset, unsigned val) {
  if (!__ByteBuffer_checkRegion(bb, offset, 4))
    return false;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    *(unsigned*)&bb->ptr[offset] = val;
  } else {
    bb->ptr[offset++] = val >> 24;
    bb->ptr[offset++] = (val >> 16) & 0xff;
    bb->ptr[offset++] = (val >> 8) & 0xff;
    bb->ptr[offset] = val & 0xff;
  }
  return true;
}

bool ByteBuffer_set64(ByteBuffer* bb, unsigned offset, uint64_t val) {
  if (!__ByteBuffer_checkRegion(bb, offset, 8))
    return false;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    *(uint64_t*)&bb->ptr[offset] = val;
  } else {
    char* p = &bb->ptr[offset];
    int steps = 8;
    while (steps-- > 0)
      p[7 - steps] = ((char*)&val)[steps];
  }
  return true;
}

unsigned ByteBuffer_set(ByteBuffer* bb, unsigned offset, const void* buf, unsigned len) {
  if (offset > bb->limit)
    return 0;
  unsigned rem = bb->limit - offset;
  if (len > rem)
    len = rem;
  if (len)
    memcpy(bb->ptr + offset, buf, len);
  return len;
}

unsigned char ByteBuffer_take8(ByteBuffer* bb) {
  LOG_ASSERT_P(__ByteBuffer_checkRegion(bb, bb->cursor, 1));
  return *(unsigned char*)(bb->ptr + (bb->cursor++));
}

unsigned short ByteBuffer_take16(ByteBuffer* bb) {
  return ByteBuffer_get16(bb, (bb->cursor += 2) - 2);
}

unsigned ByteBuffer_take32(ByteBuffer* bb) { return ByteBuffer_get32(bb, (bb->cursor += 4) - 4); }

uint64_t ByteBuffer_take64(ByteBuffer* bb) { return ByteBuffer_get64(bb, (bb->cursor += 8) - 8); }

bool ByteBuffer_take(ByteBuffer* bb, void* buf, unsigned len) {
  if (!__ByteBuffer_checkRegion(bb, bb->cursor, len))
    return false;
  memcpy(buf, bb->ptr + bb->cursor, len);
  bb->cursor += len;
  return true;
}

unsigned char ByteBuffer_get8(ByteBuffer* bb, unsigned offset) {
  LOG_ASSERT_P(__ByteBuffer_checkRegion(bb, offset, 1));
  return *(unsigned char*)(bb->ptr + offset);
}

unsigned short ByteBuffer_get16(ByteBuffer* bb, unsigned offset) {
  LOG_ASSERT_P(__ByteBuffer_checkRegion(bb, offset, 2));
  unsigned short val;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    val = *(unsigned short*)&bb->ptr[offset];
  } else {
    ((char*)&val)[0] = bb->ptr[offset + 1];
    ((char*)&val)[1] = bb->ptr[offset];
  }
  return val;
}

unsigned ByteBuffer_get32(ByteBuffer* bb, unsigned offset) {
  LOG_ASSERT_P(__ByteBuffer_checkRegion(bb, offset, 4));
  unsigned val;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    val = *(unsigned*)&bb->ptr[offset];
  } else {
    char* p = &bb->ptr[offset];
    ((char*)&val)[0] = p[3];
    ((char*)&val)[1] = p[2];
    ((char*)&val)[2] = p[1];
    ((char*)&val)[3] = p[0];
  }
  return val;
}

uint64_t ByteBuffer_get64(ByteBuffer* bb, unsigned offset) {
  LOG_ASSERT_P(__ByteBuffer_checkRegion(bb, offset, 8));
  uint64_t val;
  if (localEndian() == __ByteBuffer_info(bb)->endian) {
    val = *(uint64_t*)&bb->ptr[offset];
  } else {
    char* p = &bb->ptr[offset];
    int steps = 8;
    while (steps-- > 0)
      ((char*)&val)[steps] = p[7 - steps];
  }
  return val;
}

unsigned ByteBuffer_get(ByteBuffer* bb, unsigned offset, void* buf, unsigned len) {
  if (offset > bb->limit)
    return 0;
  unsigned rem = bb->limit - offset;
  if (len > rem)
    len = rem;
  if (len)
    memcpy(buf, bb->ptr + offset, len);
  return len;
}

void ByteBuffer_rewind(ByteBuffer* bb) {
  bb->cursor = 0;
  bb->limit = bb->cap;
}

void ByteBuffer_flip(ByteBuffer* bb) {
  bb->limit = bb->cursor;
  bb->cursor = 0;
}
