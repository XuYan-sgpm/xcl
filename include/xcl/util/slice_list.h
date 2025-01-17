#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SliceList SliceList;
SliceList* SliceList_new(unsigned item_size);
void SliceList_free(SliceList* sl);
unsigned SliceList_cap(SliceList* sl);
void* SliceList_at(SliceList* sl, unsigned i);
bool SliceList_push(SliceList* sl, const void* item);
bool SliceList_pop(SliceList* sl, void* item);
bool SliceList_insert(SliceList* sl, unsigned i, const void* items, unsigned n);
bool SliceList_insertn(SliceList* sl, unsigned i, unsigned n, const void* item);
bool SliceList_insertList(SliceList* sl, unsigned i, SliceList* o, unsigned j, unsigned n);
bool SliceList_remove(SliceList* sl, unsigned i, void* item);
bool SliceList_removen(SliceList* sl, unsigned i, unsigned n, void* items);
bool SliceList_fill(SliceList* sl, const void* items, unsigned n);
bool SliceList_filln(SliceList* sl, unsigned n, const void* item);
bool SliceList_fillList(SliceList* sl, SliceList* o, unsigned i, unsigned n);
void SliceList_clear(SliceList* sl);
unsigned SliceList_size(SliceList* sl);
bool SliceList_empty(SliceList* sl);
bool SliceList_get(SliceList* sl, unsigned i, unsigned n, void* items);

#ifdef __cplusplus
}
#endif
