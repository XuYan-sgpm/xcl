//
// Created by 14049 on 2023/12/1.
//

#include <stdlib.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/util/algo.h>
#include <xcl/util/heap.h>
#include <xcl/util/log.h>

struct Heap {
  unsigned item_size;
  char* ptr;
  unsigned size, cap;
  Comparator* comp;
  void* tmp_item;
};

static inline bool Heap_less(Heap* heap, const void* o1, const void* o2) {
  return (heap->comp ? heap->comp->__vptr->compare(heap->comp, o1, o2)
                     : memcmp(o1, o2, heap->item_size)) < 0;
}
static inline void* Heap_item(Heap* heap, unsigned pos) {
  return heap->ptr + pos * heap->item_size;
}
static inline void Heap_swap(Heap* heap, void* x, void* y) {
  memcpy(heap->tmp_item, x, heap->item_size);
  memcpy(x, y, heap->item_size);
  memcpy(y, heap->tmp_item, heap->item_size);
}

static bool Heap_init(Heap* heap, unsigned item_size, Comparator* comp) {
  static const unsigned initial_heap_cap = 8;
  memset(heap, 0, sizeof(*heap));
  heap->item_size = item_size;
  heap->ptr = palloc(initial_heap_cap * item_size);
  if (heap->ptr && (heap->tmp_item = palloc(item_size))) {
    heap->comp = comp;
    heap->cap = initial_heap_cap;
    return true;
  }
  pfree(heap->ptr);
  pfree(heap->tmp_item);
  return false;
}

Heap* Heap_new(unsigned item_size, Comparator* comp) {
  if (!item_size || !comp)
    return NULL;
  Heap* heap = palloc(sizeof(Heap));
  if (heap && Heap_init(heap, item_size, comp))
    return heap;
  pfree(heap);
  return NULL;
}

void Heap_free(Heap* heap) {
  if (heap) {
    pfree(heap->ptr);
    pfree(heap->tmp_item);
    pfree(heap);
  }
}

static void Heap_adjustUpward(Heap* heap, unsigned i) {
  while (i > 0) {
    unsigned par = (i & 1) ? (i >> 1) : ((i >> 1) - 1);
    void* p = Heap_item(heap, i);
    void* q = Heap_item(heap, par);
    if (!Heap_less(heap, p, q))
      break;
    Heap_swap(heap, p, q);
    i = par;
  }
}
static void Heap_adjustDownward(Heap* heap, unsigned i) {
  for (;;) {
    void* p = Heap_item(heap, i);
    unsigned l = (i << 1) + 1, r = ((i + 1) << 1);
    if (l >= heap->size && r >= heap->size)
      break;
    unsigned j = l;
    void* q = Heap_item(heap, l);
    if (r < heap->size && Heap_less(heap, Heap_item(heap, r), q)) {
      q = Heap_item(heap, r);
      j = r;
    }
    if (Heap_less(heap, p, q))
      break;
    Heap_swap(heap, p, q);
    i = j;
  }
}

bool Heap_push(Heap* heap, const void* item) {
  if (!Heap_reserve(heap, heap->size + 1))
    return false;
  memcpy(heap->ptr + heap->size * heap->item_size, item, heap->item_size);
  ++heap->size;
  Heap_adjustUpward(heap, heap->size - 1);
  return true;
}

bool Heap_pop(Heap* heap, void* item) {
  if (!heap->size)
    return false;
  memcpy(item, heap->ptr, heap->item_size);
  --heap->size;
  if (heap->size) {
    memcpy(heap->ptr, Heap_item(heap, heap->size), heap->item_size);
    Heap_adjustDownward(heap, 0);
  }
  return true;
}

unsigned Heap_size(Heap* heap) { return heap->size; }

bool Heap_empty(Heap* heap) { return heap->size == 0; }

void Heap_clear(Heap* heap) { heap->size = 0; }

bool Heap_reserve(Heap* heap, unsigned size) {
  if (size <= heap->cap)
    return true;
  unsigned new_cap = heap->cap << 1;
  if (new_cap < size)
    new_cap = size;
  char* ptr = palloc(new_cap * heap->item_size);
  if (!ptr)
    return false;
  if (heap->size)
    memcpy(ptr, heap->ptr, heap->size * heap->item_size);
  pfree(heap->ptr);
  heap->ptr = ptr;
  heap->cap = new_cap;
  return true;
}

bool Heap_build(Heap* heap, const void* items, unsigned len) {
  heap->size = 0;
  if (!Heap_reserve(heap, len))
    return false;
  memcpy(heap->ptr, items, len * heap->item_size);
  heap->size = len;
  for (int i = 1; i < len; i++)
    Heap_adjustUpward(heap, i);
  return true;
}

bool Heap_top(Heap* heap, void* item) {
  if (!heap->size)
    return false;
  memcpy(item, heap->ptr, heap->item_size);
  return true;
}
