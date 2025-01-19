/* This is a public domain base64 implementation written by WEI Zhicheng. */

#include <xcl/util/base64.h>

#define BASE64_PAD '='
#define BASE64DE_FIRST '+'
#define BASE64DE_LAST 'z'

/* BASE 64 encode table */
static const char base64en[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
};

/* ASCII order for BASE 64 decode, 255 in unused character */
static const unsigned char base64de[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 62,  255, 255, 255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,
    61,  255, 255, 255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,
    11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  255, 255, 255, 255,
    255, 255, 26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,
    43,  44,  45,  46,  47,  48,  49,  50,  51,  255, 255, 255, 255, 255};

typedef struct {
  unsigned char cache[4096];
  unsigned size;
  bool (*write)(void*, const void*, unsigned);
  void* ctx;
} CachedWriter;

static bool CachedWriter_flush(CachedWriter* w) {
  if (w->write(w->ctx, w->cache, w->size))
    return w->size = 0, true;
  return false;
}
static bool CachedWriter_push(CachedWriter* w, unsigned char val) {
  if (w->size == sizeof(w->cache) && !CachedWriter_flush(w))
    return false;
  return w->cache[w->size++] = val, true;
}

unsigned int base64_encode(const unsigned char* in, unsigned int inlen, char* out) {
  int s;
  unsigned int i;
  unsigned int j;
  unsigned char c;
  unsigned char l;

  s = 0;
  l = 0;
  for (i = j = 0; i < inlen; i++) {
    c = in[i];

    switch (s) {
    case 0:
      s = 1;
      out[j++] = base64en[(c >> 2) & 0x3F];
      break;
    case 1:
      s = 2;
      out[j++] = base64en[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
      break;
    case 2:
      s = 0;
      out[j++] = base64en[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
      out[j++] = base64en[c & 0x3F];
      break;
    }
    l = c;
  }

  switch (s) {
  case 1:
    out[j++] = base64en[(l & 0x3) << 4];
    out[j++] = BASE64_PAD;
    out[j++] = BASE64_PAD;
    break;
  case 2:
    out[j++] = base64en[(l & 0xF) << 2];
    out[j++] = BASE64_PAD;
    break;
  }

  out[j] = 0;

  return j;
}

bool base64_encode2(const unsigned char* in, unsigned int inlen,
                    bool (*write)(void*, const void*, unsigned), void* ctx) {
  CachedWriter w = {.cache = {0}, .size = 0, .ctx = ctx, .write = write};
  int s;
  unsigned int i;
  unsigned char c;
  unsigned char l;

  bool ret = true;
  s = 0;
  l = 0;
  for (i = 0; i < inlen && ret; i++) {
    c = in[i];
    switch (s) {
    case 0: {
      ret = CachedWriter_push(&w, base64en[(c >> 2) & 0x3F]);
      break;
    }
    case 1: {
      ret = CachedWriter_push(&w, base64en[((l & 0x3) << 4) | ((c >> 4) & 0xF)]);
      break;
    }
    case 2: {
      ret = CachedWriter_push(&w, base64en[((l & 0xF) << 2) | ((c >> 6) & 0x3)]) &&
            CachedWriter_push(&w, base64en[c & 0x3F]);
      break;
    }
    }
    l = c;
    s = (s + 1) % 3;
  }
  if (ret) {
    switch (s) {
    case 1:
      ret = CachedWriter_push(&w, base64en[(l & 0x3) << 4]) && CachedWriter_push(&w, BASE64_PAD) &&
            CachedWriter_push(&w, BASE64_PAD);
      break;
    case 2:
      ret = CachedWriter_push(&w, base64en[(l & 0xF) << 2]) && CachedWriter_push(&w, BASE64_PAD);
      break;
    }
    ret = ret && CachedWriter_flush(&w);
  }
  return ret;
}

unsigned int base64_decode(const char* in, unsigned int inlen, unsigned char* out) {
  unsigned int i;
  unsigned int j;
  unsigned char c;

  if (inlen & 0x3) {
    return 0;
  }

  for (i = j = 0; i < inlen; i++) {
    if (in[i] == BASE64_PAD) {
      break;
    }
    if (in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST) {
      return 0;
    }

    c = base64de[(unsigned char)in[i]];
    if (c == 255) {
      return 0;
    }

    switch (i & 0x3) {
    case 0:
      out[j] = (c << 2) & 0xFF;
      break;
    case 1:
      out[j++] |= (c >> 4) & 0x3;
      out[j] = (c & 0xF) << 4;
      break;
    case 2:
      out[j++] |= (c >> 2) & 0xF;
      out[j] = (c & 0x3) << 6;
      break;
    case 3:
      out[j++] |= c;
      break;
    }
  }

  return j;
}

bool base64_decode2(const char* in, unsigned int inlen, bool (*write)(void*, const void*, unsigned),
                    void* ctx) {
  unsigned int i;
  unsigned int j;
  unsigned char c;

  CachedWriter w = {.cache = {0}, .size = 0, .ctx = ctx, .write = write};
  unsigned char val = 0;

  if (inlen & 0x3)
    return false;

  bool ret = true;
  for (i = j = 0; i < inlen && ret; i++) {
    if (in[i] == BASE64_PAD)
      break;
    if (in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST)
      return false;

    c = base64de[(unsigned char)in[i]];
    if (c == 255)
      return false;

    switch (i & 0x3) {
    case 0:
      val = (c << 2) & 0xFF;
      break;
    case 1:
      ret = CachedWriter_push(&w, val | ((c >> 4) & 0x3));
      val = (c & 0xF) << 4;
      break;
    case 2:
      ret = CachedWriter_push(&w, val | ((c >> 2) & 0xF));
      val = (c & 0x3) << 6;
      break;
    case 3:
      ret = CachedWriter_push(&w, val | c);
      break;
    }
  }

  return ret && CachedWriter_flush(&w);
}
