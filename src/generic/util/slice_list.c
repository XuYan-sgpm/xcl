#include <stdbool.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/slice_list.h>

#define SLICE_CAP_SHIFT 9
#define SLICE_CAP (1 << SLICE_CAP_SHIFT)
#define SLICE_CAP_MASK (SLICE_CAP - 1)

typedef struct {
  char* buf;
} Slice;

struct SliceList {
  Slice* slices;
  unsigned slice_size, max_slice;
  unsigned size;
  unsigned item_size;
};

static Slice SliceList_newSlice(SliceList* sl) {
  Slice slice = {.buf = palloc(sl->item_size << SLICE_CAP_SHIFT)};
  return slice;
}
static inline unsigned SliceList_slices(unsigned n) {
  return (n + SLICE_CAP_MASK) >> SLICE_CAP_SHIFT;
}
static inline void* SliceList_item(SliceList* sl, unsigned i) {
  return &sl->slices[i >> SLICE_CAP_SHIFT].buf[(i & SLICE_CAP_MASK) * sl->item_size];
}

SliceList* SliceList_new(unsigned item_size) {
  SliceList* sl = palloc(sizeof(SliceList));
  if (sl) {
    memset(sl, 0, sizeof(SliceList));
    sl->max_slice = 8;
    sl->slices = palloc(8 * sizeof(void*));
    if (sl->slices) {
      sl->item_size = item_size;
      return sl;
    }
    pfree(sl);
  }
  return NULL;
}

unsigned SliceList_cap(SliceList* sl) { return sl->slice_size << SLICE_CAP_SHIFT; }

void* SliceList_at(SliceList* sl, unsigned i) {
  return i >= sl->size ? NULL : SliceList_item(sl, i);
}

static bool SliceList_expand(SliceList* sl, unsigned required) {
  if (required <= sl->slice_size)
    return true;
  if (required > sl->max_slice) {
    unsigned new_cap = sl->max_slice << 1;
    if (new_cap < required)
      new_cap = required;
    Slice* new_slices = palloc(new_cap * sizeof(void*));
    if (!new_slices)
      return false;
    memcpy(new_slices, sl->slices, sl->slice_size * sizeof(void*));
    free(sl->slices);
    sl->slices = new_slices;
    sl->max_slice = new_cap;
  }
  while (sl->slice_size < required) {
    Slice slice = SliceList_newSlice(sl);
    if (!slice.buf)
      return false;
    sl->slices[sl->slice_size++] = slice;
  }
  return true;
}

static void SliceList_moveForward(SliceList* sl, unsigned i, unsigned j, unsigned len) {
  while (len > 0) {
    Slice *si = &sl->slices[i >> SLICE_CAP_SHIFT], *sj = &sl->slices[j >> SLICE_CAP_SHIFT];
    unsigned pos_i = i & SLICE_CAP_MASK, pos_j = j & SLICE_CAP_MASK;
    unsigned cpy_len = SLICE_CAP - (pos_i >= pos_j ? pos_i : pos_j);
    if (cpy_len > len)
      cpy_len = len;
    memmove(&si->buf[pos_i * sl->item_size], &sj->buf[pos_j * sl->item_size],
            cpy_len * sl->item_size);
    i += cpy_len, j += cpy_len;
    len -= cpy_len;
  }
}
static void SliceList_moveBackward(SliceList* sl, unsigned i, unsigned j, unsigned len) {
  j += len, i += len;
  while (len > 0) {
    Slice *si = &sl->slices[i >> SLICE_CAP_SHIFT], *sj = &sl->slices[j >> SLICE_CAP_SHIFT];
    unsigned pos_i = i & SLICE_CAP_MASK, pos_j = j & SLICE_CAP_MASK;
    if (!pos_i)
      pos_i = SLICE_CAP, --si;
    if (!pos_j)
      pos_j = SLICE_CAP, --sj;
    unsigned cpy_len = pos_i >= pos_j ? pos_j : pos_i;
    if (cpy_len > len)
      cpy_len = len;
    memmove(&si->buf[(pos_i - cpy_len) * sl->item_size],
            &sj->buf[(pos_j - cpy_len) * sl->item_size], cpy_len * sl->item_size);
    i -= cpy_len, j -= cpy_len;
    len -= cpy_len;
  }
}
static void SliceList_move(SliceList* sl, unsigned i, unsigned j, unsigned len) {
  if (i == j || !len)
    return;
  (i < j ? SliceList_moveForward : SliceList_moveBackward)(sl, i, j, len);
}

static void SliceList_cpyList(SliceList* sl, unsigned i, SliceList* o, unsigned j, unsigned n) {
  if (sl == o) {
    SliceList_move(sl, i, j, n);
    return;
  }
  while (n > 0) {
    Slice *si = &sl->slices[i >> SLICE_CAP_SHIFT], *sj = &o->slices[j >> SLICE_CAP_SHIFT];
    unsigned pos_i = i & SLICE_CAP_MASK, pos_j = j & SLICE_CAP_MASK;
    unsigned cpy_len = SLICE_CAP - (pos_i >= pos_j ? pos_i : pos_j);
    if (cpy_len > n)
      cpy_len = n;
    memmove(&si->buf[pos_i * sl->item_size], &sj->buf[pos_j * sl->item_size],
            cpy_len * sl->item_size);
    i += cpy_len, j += cpy_len;
    n -= cpy_len;
  }
}

static void SliceList_cpy(SliceList* sl, unsigned i, const void* items, unsigned n) {
  while (n > 0) {
    Slice* si = &sl->slices[i >> SLICE_CAP_SHIFT];
    unsigned pos_i = i & SLICE_CAP_MASK;
    unsigned cpy_len = SLICE_CAP - pos_i;
    if (cpy_len > n)
      cpy_len = n;
    memmove(&si->buf[pos_i * sl->item_size], items, cpy_len * sl->item_size);
    i += cpy_len;
    items = (char*)items + cpy_len * sl->item_size;
    n -= cpy_len;
  }
}

static void SliceList_setn(SliceList* sl, unsigned i, unsigned n, const void* item) {
  while (n > 0) {
    Slice* si = &sl->slices[i >> SLICE_CAP_SHIFT];
    unsigned pos_i = i & SLICE_CAP_MASK;
    int cpy_len = SLICE_CAP - pos_i;
    if (cpy_len > n)
      cpy_len = n;
    i += cpy_len;
    n -= cpy_len;
    while (cpy_len--)
      memcpy(&si->buf[(pos_i++) * sl->item_size], item, sl->item_size);
  }
}

static void SliceList_write(SliceList* sl, unsigned i, unsigned n, void* items) {
  while (n > 0) {
    Slice* si = &sl->slices[i >> SLICE_CAP_SHIFT];
    unsigned pos_i = i & SLICE_CAP_MASK;
    unsigned cpy_len = SLICE_CAP - pos_i;
    if (cpy_len > n)
      cpy_len = n;
    memmove(items, &si->buf[pos_i * sl->item_size], cpy_len * sl->item_size);
    i += cpy_len;
    items = (char*)items + cpy_len * sl->item_size;
    n -= cpy_len;
  }
}

static void* SliceList_preInsert(SliceList* sl, unsigned i, unsigned n) {
  unsigned new_size = sl->size + n;
  if (new_size > SliceList_cap(sl) && !SliceList_expand(sl, SliceList_slices(new_size)))
    return NULL;
  SliceList_move(sl, i + n, i, sl->size - i);
  sl->size = new_size;
  return SliceList_item(sl, i);
}

static void* SliceList_preFill(SliceList* sl, unsigned n) {
  if (SliceList_cap(sl) < n && !SliceList_expand(sl, SliceList_slices(n)))
    return NULL;
  sl->size = n;
  return sl->slices[0].buf;
}

bool SliceList_push(SliceList* sl, const void* item) {
  void* p = SliceList_preInsert(sl, sl->size, 1);
  if (p)
    memcpy(p, item, sl->item_size);
  return p;
}

bool SliceList_pop(SliceList* sl, void* item) {
  return sl->size ? (memcpy(item, SliceList_item(sl, --sl->size), sl->item_size), true) : false;
}

bool SliceList_insert(SliceList* sl, unsigned i, const void* items, unsigned n) {
  return SliceList_preInsert(sl, i, n) ? (SliceList_cpy(sl, i, items, n), true) : false;
}

bool SliceList_insertn(SliceList* sl, unsigned i, unsigned n, const void* item) {
  return SliceList_preInsert(sl, i, n) ? (SliceList_setn(sl, i, n, item), true) : false;
}

bool SliceList_insertList(SliceList* sl, unsigned i, SliceList* o, unsigned j, unsigned n) {
  if (i > sl->size || j > o->size || j + n > o->size)
    return false;
  return SliceList_preInsert(sl, i, n) ? (SliceList_cpyList(sl, i, o, j, n), true) : false;
}

static void SliceList_finishRemove(SliceList* sl, unsigned n) {
  unsigned slice_size = SliceList_slices(sl->size - n);
  for (unsigned i = sl->slice_size - slice_size; i < sl->slice_size; i++)
    pfree(sl->slices[i].buf);
  sl->slice_size = slice_size;
  sl->size -= n;
}

bool SliceList_remove(SliceList* sl, unsigned i, void* item) {
  if (i >= sl->size)
    return false;
  if (item)
    memcpy(item, SliceList_item(sl, i), sl->item_size);
  SliceList_cpyList(sl, i, sl, i + 1, sl->size - i - 1);
  SliceList_finishRemove(sl, 1);
  return true;
}

bool SliceList_removen(SliceList* sl, unsigned i, unsigned n, void* items) {
  if (i + n > sl->size)
    return false;
  if (items)
    SliceList_write(sl, i, n, items);
  SliceList_move(sl, i, i + n, sl->size - i - n);
  SliceList_finishRemove(sl, n);
  return true;
}

bool SliceList_fill(SliceList* sl, const void* items, unsigned n) {
  return SliceList_preFill(sl, n) ? (SliceList_cpy(sl, 0, items, n), true) : false;
}

bool SliceList_filln(SliceList* sl, unsigned n, const void* item) {
  return SliceList_preFill(sl, n) ? (SliceList_setn(sl, 0, n, item), true) : false;
}

bool SliceList_fillList(SliceList* sl, SliceList* o, unsigned i, unsigned n) {
  if (i + n > o->size)
    return false;
  return SliceList_preFill(sl, n) ? (SliceList_cpyList(sl, 0, o, i, n), true) : false;
}

void SliceList_clear(SliceList* sl) {
  sl->size = 0;
  for (unsigned i = 0; i < sl->slice_size; i++)
    pfree(sl->slices[i].buf);
  sl->slice_size = 0;
}

void SliceList_free(SliceList* sl) {
  if (sl) {
    SliceList_clear(sl);
    pfree(sl->slices);
    pfree(sl);
  }
}

unsigned SliceList_size(SliceList* sl) { return sl->size; }

bool SliceList_empty(SliceList* sl) { return sl->size == 0; }

bool SliceList_get(SliceList* sl, unsigned i, unsigned n, void* items) {
  if (i + n > sl->size)
    return false;
  SliceList_write(sl, i, n, items);
  return true;
}
