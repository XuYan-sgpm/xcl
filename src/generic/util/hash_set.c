//
// Created by 徐琰 on 2023/11/17.
//

#include <internal/util/hash_table.h>
#include <stddef.h>
#include <string.h>
#include <xcl/util/hash_set.h>
#include <xcl/util/log.h>

void* HashSet_new(unsigned item_size, struct Hasher* hasher, Comparator* comp) {
  return HashTable_new(item_size, item_size, hasher, comp, NULL);
}

void HashSet_free(void* s) { HashTable_free(s); }

HashIter HashSet_add(void* s, const void* item) { return HashTable_addUnique(s, item); }

HashIter HashSet_find(void* s, const void* item) { return HashTable_find(s, item); }

bool HashSet_remove(void* s, const void* item) { return HashTable_remove(s, item); }

bool HashSet_removeAt(void* s, HashIter iter) { return HashTable_removeAt(s, iter); }

unsigned HashSet_size(void* s) { return HashTable_size(s); }

void HashSet_clear(void* s) { HashTable_clear(s); }

bool HashSet_empty(void* s) { return HashTable_empty(s); }

HashIter HashSet_begin(void* s) { return HashTable_begin(s); }

HashIter HashSet_end(void* s) { return HashTable_end(s); }
