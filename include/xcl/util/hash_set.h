//
// Created by 徐琰 on 2023/11/17.
//

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <xcl/lang/define.h>
#include <xcl/util/algo.h>
#include <xcl/util/comparator.h>
#include <xcl/util/hash_iter.h>

#ifdef __cplusplus
extern "C" {
#endif

void* HashSet_new(unsigned item_size, struct Hasher* hasher, Comparator* comp);
void HashSet_free(void* s);
HashIter HashSet_add(void* s, const void* item);
HashIter HashSet_find(void* s, const void* item);
bool HashSet_remove(void* s, const void* item);
bool HashSet_removeAt(void* s, HashIter iter);
unsigned HashSet_size(void* s);
void HashSet_clear(void* s);
bool HashSet_empty(void* s);
HashIter HashSet_begin(void* s);
HashIter HashSet_end(void* s);

#ifdef __cplusplus
};
#endif
