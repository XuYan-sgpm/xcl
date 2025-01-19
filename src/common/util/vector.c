//
// Created by 徐琰 on 2023/11/16.
//

#include <stdlib.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/vector.h>

struct Vector {
  char* buf;
  unsigned size, cap;
  unsigned item_size;
};

static inline void* Vec_item(Vector* vec, unsigned pos) { return vec->buf + pos * vec->item_size; }

static bool Vec_init(Vector* vec, unsigned cap, unsigned item_size) {
  memset(vec, 0, sizeof(*vec));
  if (!item_size || !cap)
    return false;
  vec->item_size = item_size;
  vec->buf = palloc(cap * item_size);
  if (!vec->buf)
    return false;
  vec->cap = cap;
  return true;
}

Vector* Vec_new2(unsigned item_size, unsigned cap) {
  Vector* vec = palloc(sizeof(Vector));
  if (vec && Vec_init(vec, cap, item_size))
    return vec;
  pfree(vec);
  return NULL;
}

Vector* Vec_new(unsigned item_size) { return Vec_new2(item_size, 8); }

void Vec_free(Vector* vec) {
  if (vec) {
    pfree(vec->buf);
    pfree(vec);
  }
}

bool Vec_push(Vector* vec, const void* item) {
  if (!Vec_reserve(vec, vec->size + 1))
    return false;
  memcpy(Vec_item(vec, vec->size++), item, vec->item_size);
  return true;
}

bool Vec_pop(Vector* vec, void* item) {
  if (!vec->size)
    return false;
  if (item)
    memcpy(item, Vec_item(vec, --vec->size), vec->item_size);
  return true;
}

unsigned Vec_size(Vector* vec) { return vec->size; }

bool Vec_empty(Vector* vec) { return vec->size == 0; }

unsigned Vec_cap(Vector* vec) { return vec->cap; }

static void* Vec_preIns(Vector* vec, unsigned pos, unsigned n) {
  if (pos > vec->size)
    return NULL;
  if (n == 0)
    return Vec_item(vec, pos);
  if (vec->size + n <= vec->cap) {
    memmove(Vec_item(vec, pos + n), Vec_item(vec, pos), (vec->size - pos) * vec->item_size);
  } else {
    unsigned new_cap = vec->cap << 1;
    unsigned req = vec->size + n;
    if (new_cap < req)
      new_cap = req;
    char* new_buf = palloc(vec->item_size * new_cap);
    if (!new_cap)
      return NULL;
    memcpy(new_buf, vec->buf, vec->item_size * pos);
    memcpy(new_buf + (pos + n) * vec->item_size, Vec_item(vec, pos),
           (vec->size - pos) * vec->item_size);
    pfree(vec->buf);
    vec->buf = new_buf;
    vec->cap = new_cap;
  }
  vec->size += n;
  return Vec_item(vec, pos);
}

bool Vec_addn(Vector* vec, unsigned pos, unsigned n, const void* item) {
  char* p = Vec_preIns(vec, pos, n);
  if (!p)
    return false;
  for (int i = 0; i < n; i++)
    memcpy(p + (i * vec->item_size), item, vec->item_size);
  return true;
}

bool Vec_add(Vector* vec, unsigned pos, const void* start, const void* finish) {
  unsigned n = ((const char*)finish - (const char*)start) / vec->item_size;
  char* p = Vec_preIns(vec, pos, n);
  if (!p)
    return false;
  memcpy(p, start, n * vec->item_size);
  return true;
}

bool Vec_remove(Vector* vec, unsigned pos, unsigned n) {
  if (pos + n > vec->size)
    return false;
  if (pos + n < vec->size)
    memmove(Vec_item(vec, pos), Vec_item(vec, pos + n), (vec->size - pos - n) * vec->item_size);
  vec->size -= n;
  return true;
}

bool Vec_filln(Vector* vec, unsigned n, const void* item) {
  if (!Vec_reserve(vec, n))
    return false;
  for (int i = 0; i < n; i++)
    memcpy(Vec_item(vec, i), item, vec->item_size);
  vec->size = n;
  return true;
}

bool Vec_fill(Vector* vec, const void* start, const void* finish) {
  unsigned n = ((const char*)finish - (const char*)start) / vec->item_size;
  if (!Vec_reserve(vec, n))
    return false;
  memcpy(vec->buf, start, n * vec->item_size);
  vec->size = n;
  return true;
}

void Vec_clear(Vector* vec) { vec->size = 0; }

bool Vec_reserve(Vector* vec, unsigned size) {
  if (vec->cap >= size)
    return true;
  unsigned new_cap = vec->cap << 1;
  if (new_cap < size)
    new_cap = size;
  char* new_buf = palloc(new_cap * vec->item_size);
  if (!new_buf)
    return false;
  memcpy(new_buf, vec->buf, vec->size * vec->item_size);
  pfree(vec->buf);
  vec->buf = new_buf;
  vec->cap = new_cap;
  return true;
}

Vector* Vec_copy(const Vector* vec) {
  Vector* o = palloc(sizeof(Vector));
  if (o && Vec_init(o, vec->cap, vec->item_size)) {
    memcpy(o->buf, vec->buf, vec->size * vec->item_size);
    o->size = vec->size;
    return o;
  }
  pfree(o);
  return NULL;
}

void* Vec_pos(Vector* vec, unsigned pos) { return pos >= vec->size ? NULL : Vec_item(vec, pos); }

bool Vec_resize(Vector* vec, unsigned n, const void* item) {
  if (n > vec->size) {
    void* p = Vec_preIns(vec, vec->size, n - vec->size);
    if (!p)
      return false;
    int fill_count = n - vec->size;
    while (fill_count--) {
      memcpy(p, item, vec->item_size);
      p = (char*)p + vec->item_size;
    }
  }
  vec->size = n;
  return true;
}

void* Vec_back(Vector* vec) { return vec->size > 0 ? Vec_item(vec, vec->size - 1) : NULL; }
