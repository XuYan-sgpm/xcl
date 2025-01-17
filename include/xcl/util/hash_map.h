//
// Created by 徐琰 on 2023/11/23.
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

void* HashMap_new(unsigned key_size, unsigned value_size, struct Hasher* key_hasher,
                  Comparator* key_comp);
void HashMap_free(void* m);
HashIter HashMap_add(void* m, const void* key, const void* value);
bool HashMap_remove(void* m, const void* key);
bool HashMap_removeAt(void* m, HashIter iter);
unsigned HashMap_size(void* m);
void HashMap_clear(void* m);
bool HashMap_empty(void* m);
HashIter HashMap_find(void* m, const void* key);
HashIter HashMap_begin(void* m);
HashIter HashMap_end(void* m);

#ifdef __cplusplus
};
#endif
