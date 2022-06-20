#include "util/CBuffer.h"

// #include <stdarg.h>
#include <stdlib.h>
#include <string.h>

const static unsigned __BUF_MASK__ = 0x7fffffff;

static inline bool __isBufReleasable(CBuffer *buffer) {
  return (buffer->state & ~__BUF_MASK__) >> 31;
}

static void __setBufState(CBuffer *buffer, int cap, const bool flag) {
  int val = flag;
  buffer->state = (cap & __BUF_MASK__) | (val << 31);
}

CBuffer makeBuf(int cap) {
  CBuffer buffer = {NULL, 0, 0};
  if (cap > 0) {
    char *p = (char *)malloc(cap);
    if (p) {
      buffer.data = p;
      __setBufState(&buffer, cap, true);
    }
  }
  return buffer;
}

CBuffer makeBuf2(char *src, int len) {
  CBuffer buffer = {NULL, 0, 0};
  if (src && len > 0) {
    buffer.data = src;
    __setBufState(&buffer, len, false);
  }
  return buffer;
}

int bufCap(const CBuffer *buffer) { return buffer->state & __BUF_MASK__; }

CBuffer wrapBuf(char *src, int len) {
  CBuffer buffer = {src, 0, len};
  __setBufState(&buffer, len, false);
  return buffer;
}

CBuffer wrapBuf2(const CBuffer *origin, int pos, int len) {
  CBuffer buffer = {NULL, 0, 0};
  if (origin && pos >= 0 && len >= 0 && pos + len <= origin->size) {
    buffer.size = len;
    buffer.data = &origin->data[pos];
    __setBufState(&buffer, len, false);
  }
  return buffer;
}

bool bufFree(CBuffer *buffer) {
  if (buffer->data && __isBufReleasable(buffer)) {
    free(buffer->data);
    memset(buffer, 0, sizeof(*buffer));
    return true;
  }
  return false;
}

bool bufPush(CBuffer *buffer, char ch) {
  if (buffer->size < bufCap(buffer)) {
    buffer->data[buffer->size++] = ch;
    return true;
  }
  return false;
}

int bufAppendRegion(CBuffer *buffer, const char *src, int len) {
  if (len <= 0 || !src) {
    return 0;
  }
  int write = bufCap(buffer) - buffer->size;
  if (write > len) {
    write = len;
  }
  memcpy(buffer->data + buffer->size, src, write);
  buffer->size += write;
  return write;
}

int bufAppend(CBuffer *buffer, const char *src) {
  return src ? bufAppendRegion(buffer, src, strlen(src)) : 0;
}

int bufAppendChars(CBuffer *buffer, int n, char ch) {
  if (n <= 0) {
    return 0;
  }
  int count = bufCap(buffer) - buffer->size;
  if (count > n) {
    count = n;
  }
  memset(buffer->data + buffer->size, count, ch);
  buffer->size += count;
  return count;
}

bool bufPop(CBuffer *buffer, char *dst) {
  if (buffer->size) {
    if (dst) {
      *dst = buffer->data[0];
    }
    if (buffer->size > 1) {
      memcpy(buffer->data, buffer->data + 1, buffer->size - 1);
    }
    --buffer->size;
    return true;
  }
  return false;
}

bool bufGet(const CBuffer *buffer, int pos, char *dst) {
  if (pos < 0 || pos >= buffer->size) {
    return false;
  }
  if (dst) {
    *dst = buffer->data[pos];
  }
  return true;
}

bool bufWriteChar(CBuffer *buffer, int pos, char ch) {
  if (pos >= 0 && pos <= buffer->size && buffer->size < bufCap(buffer)) {
    if (pos < buffer->size) {
      memmove(buffer->data + pos + 1, buffer->data + pos, buffer->size - pos);
    }
    buffer->data[pos] = ch;
    ++buffer->size;
    return true;
  }
  return false;
}

bool bufWriteChars(CBuffer *buffer, int pos, int n, char ch) {
  if (pos >= 0 && n >= 0 && buffer->size + n <= bufCap(buffer)) {
    if (pos < buffer->size) {
      memmove(buffer->data + pos + n, buffer->data + pos, buffer->size - pos);
    }
    memset(buffer->data + pos, n, ch);
    buffer->size += n;
    return true;
  }
  return false;
}

int bufWriteRegion(CBuffer *buffer, int pos, const char *src, int len) {
  if (pos > buffer->size || len <= 0 || !src) {
    return 0;
  }
  int remaining = bufCap(buffer) - buffer->size;
  int write = remaining > len ? len : remaining;
  if (pos < buffer->size)
    memmove(&buffer->data[pos + write], &buffer->data[pos], buffer->size - pos);
  memcpy(buffer->data + pos, src, len);
  buffer->size += len;
  return write;
}

int bufWrite(CBuffer *buffer, int pos, const char *src) {
  return src ? bufWriteRegion(buffer, pos, src, strlen(src)) : 0;
}

int bufRead2(const CBuffer *buffer, int pos, char *dst, int len) {
  if (pos >= buffer->size || len <= 0) {
    return 0;
  }
  int available = buffer->size - pos;
  int read = available > len ? len : available;
  if (read > 1) {
    memcpy(dst, buffer->data + pos, read);
  } else {
    *dst = buffer->data[pos];
  }
  return read;
}

int bufRead(const CBuffer *buffer, int pos, char *dst) {
  if (buffer->size - pos > 0)
    return bufRead2(buffer, pos, dst, buffer->size - pos);
  return 0;
}

bool bufExpand(CBuffer *buffer, int cap) {
  if (cap <= bufCap(buffer)) {
    //
  } else if (!buffer->data || !__isBufReleasable(buffer)) {
    CBuffer newBuf = makeBuf(cap);
    if (!newBuf.data) {
      return false;
    }
    *buffer = newBuf;
  } else {
    char *p = realloc(buffer->data, cap);
    if (!p) {
      CBuffer newBuf = makeBuf(cap);
      if (newBuf.data) {
        memcpy(newBuf.data, buffer->data, buffer->size);
        newBuf.size = buffer->size;
        free(buffer->data);
      } else {
        return false;
      }
      *buffer = newBuf;
    } else {
      buffer->data = p;
      __setBufState(buffer, cap, true);
    }
  }
  return true;
}

void bufRemoveRegion(CBuffer *buffer, int pos, int len) {
  if (pos >= 0 && len >= 0 && pos + len <= buffer->size) {
    if (pos + len < buffer->size) {
      memcpy(buffer->data + pos,
             buffer->data + (pos + len),
             buffer->size - pos - len);
    }
    buffer->size -= len;
  }
}

void bufRemovePos(CBuffer *buffer, int pos) { bufRemoveRegion(buffer, pos, 1); }

void bufClear(CBuffer *buffer) {
  if (buffer->size > 0) {
    buffer->size = 0;
  }
}

bool bufRealloc(CBuffer *buffer, int cap) {
  if (cap < 0) {
    return false;
  }
  if (cap == 0) {
    return bufFree(buffer);
  } else if (!__isBufReleasable(buffer)) {
    CBuffer newBuf = makeBuf(cap);
    if (!newBuf.data) {
      return false;
    }
    int size = cap > buffer->size ? buffer->size : cap;
    memcpy(newBuf.data, buffer->data, size);
    newBuf.size = size;
    *buffer = newBuf;
  } else if (cap > bufCap(buffer)) {
    char *p = (char *)realloc(buffer->data, cap);
    if (!p) {
      return false;
    }
    buffer->data = p;
    __setBufState(buffer, cap, true);
  }
  return true;
}

char *bufAt(const CBuffer *buffer, int pos) {
  if (pos >= 0 && pos <= buffer->size) {
    return buffer->data + pos;
  }
  return NULL;
}
