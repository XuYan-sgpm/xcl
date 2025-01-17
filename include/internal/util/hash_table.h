//
// Created by 徐琰 on 2023/11/14.
//

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <xcl/lang/define.h>
#include <xcl/util/algo.h>
#include <xcl/util/comparator.h>
#include <xcl/util/hash_iter.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HashEntry {
  struct HashEntry* next;
  uint64_t hash_value;
};
typedef struct HashEntry HashEntry;

inline always_inline void* HashEntry_value(void* entry) { return (char*)entry + sizeof(HashEntry); }

struct _aligned(64) HashTable {
  struct Hasher* key_hasher;
  Comparator* key_comp;
  HashEntry** buckets;
  unsigned size, cap;
  HashEntry* header;
  unsigned item_size, key_size;
  const void* (*key_handler)(const void*);
};
typedef struct HashTable HashTable;

void HashTable_init(HashTable* hash_table, unsigned key_size, unsigned item_size,
                    struct Hasher* key_hasher, struct Comparator* key_comp,
                    const void* (*key_handler)(const void*));
HashTable* HashTable_new(unsigned key_size, unsigned item_size, struct Hasher* key_hasher,
                         struct Comparator* key_comp, const void* (*key_handler)(const void*));
void HashTable_destroy(HashTable* hash_table);
void HashTable_free(HashTable* hash_table);
inline always_inline bool HashTable_empty(HashTable* hash_table) {
  return hash_table->size == 0 && !hash_table->header->next;
}
inline always_inline unsigned HashTable_size(HashTable* hash_table) { return hash_table->size; }
HashIter HashTable_reserve(HashTable* hash_table, bool unique, const void* key);
HashIter HashTable_add(HashTable* hash_table, const void* item);
HashIter HashTable_addUnique(HashTable* hash_table, const void* item);
bool HashTable_remove(HashTable* hash_table, const void* key);
bool HashTable_removeAt(HashTable* hash_table, HashIter it);
HashIter HashTable_find(HashTable* hash_table, const void* key);
void HashTable_clear(HashTable* hash_table);
HashIter HashTable_begin(HashTable* hash_table);
HashIter HashTable_end(HashTable* hash_table);

#ifdef __cplusplus
};
#endif
