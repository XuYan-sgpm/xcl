//
// Created by 徐琰 on 2023/11/23.
//

#include <internal/util/hash_table.h>
#include <stddef.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/hash_map.h>
#include <xcl/util/log.h>

void* HashMap_new(unsigned key_size, unsigned value_size, struct Hasher* key_hasher,
                  Comparator* key_comp) {
  return HashTable_new(key_size, key_size + value_size, key_hasher, key_comp, NULL);
}

void HashMap_free(void* obj) { HashTable_free(obj); }

HashIter HashMap_add(void* obj, const void* key, const void* value) {
  unsigned item_size = ((HashTable*)obj)->item_size;
  HashIter it = HashTable_reserve(obj, true, key);
  if (HashIter_valid(it)) {
    unsigned key_size = ((HashTable*)obj)->key_size;
    void* p = HashEntry_value(it.prev->next);
    memcpy(p, key, key_size);
    memcpy((char*)p + key_size, value, item_size - key_size);
  }
  return it;
}

bool HashMap_remove(void* obj, const void* key) { return HashTable_remove(obj, key); }

bool HashMap_removeAt(void* obj, HashIter iter) { return HashTable_removeAt(obj, iter); }

unsigned HashMap_size(void* obj) { return HashTable_size(obj); }

void HashMap_clear(void* obj) { HashTable_clear(obj); }

bool HashMap_empty(void* obj) { return HashTable_empty(obj); }

HashIter HashMap_find(void* obj, const void* key) { return HashTable_find(obj, key); }

HashIter HashMap_begin(void* obj) { return HashTable_begin(obj); }

HashIter HashMap_end(void* obj) { return HashTable_end(obj); }
