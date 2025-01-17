//
// Created by 14049 on 2024/2/21.
//

#pragma once

#include <stdbool.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  struct HashTable* hash_table;
  struct HashEntry* prev;
} HashIter;

void* HashIter_key(HashIter iter);
void* HashIter_value(HashIter iter);
HashIter HashIter_next(HashIter iter);
bool HashIter_valid(HashIter iter);
bool HashIter_equal(HashIter iter1, HashIter iter2);

#ifdef __cplusplus
}
#endif
