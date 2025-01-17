//
// Created by 14049 on 2023/12/1.
//

#pragma once

#include <stdbool.h>
#include <xcl/lang/define.h>
#include <xcl/util/comparator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Heap Heap;
Heap* Heap_new(unsigned item_size, Comparator* comp);
void Heap_free(Heap* heap);
bool Heap_push(Heap* heap, const void* item);
bool Heap_pop(Heap* heap, void* item);
unsigned Heap_size(Heap* heap);
bool Heap_empty(Heap* heap);
void Heap_clear(Heap* heap);
bool Heap_reserve(Heap* heap, unsigned size);
bool Heap_build(Heap* heap, const void* buf, unsigned len);
bool Heap_top(Heap* heap, void* item);

#ifdef __cplusplus
}
#endif
