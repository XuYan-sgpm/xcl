//
// Created by 14049 on 2024/6/24.
//

#pragma once

#include <stdbool.h>
#include <xcl/util/comparator.h>
#include <xcl/util/tree_iter.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Set Set;
Set* Set_new(unsigned item_size, Comparator* comparator);
void Set_free(Set* set);
Set* Set_copy(Set* set);
bool Set_move(Set* set, Set* obj);
unsigned Set_size(Set* set);
bool Set_empty(Set* set);
TreeIter Set_add(Set* set, bool unique, const void* item);
bool Set_remove(Set* set, const void* item);
void Set_removeIter(Set* set, TreeIter iter);
TreeIter Set_begin(Set* set);
TreeIter Set_end(Set* set);
TreeIter Set_find(Set* set, const void* item);
void Set_range(Set* set, const void* item, TreeIter out[2]);
void Set_clear(Set* set);
void Set_swap(Set* set1, Set* set2);
void Set_verify(Set* set);
bool Set_equal(Set* set1, Set* set2);

#ifdef __cplusplus
}
#endif
