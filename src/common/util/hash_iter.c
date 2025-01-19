//
// Created by 14049 on 2024/2/21.
//

#include <internal/util/hash_table.h>
#include <xcl/util/algo.h>
#include <xcl/util/hash_iter.h>

HashIter HashIter_make(HashTable* hash_table, HashEntry* prev) {
  HashIter it = {.hash_table = hash_table, .prev = prev};
  return it;
}
void* HashIter_key(HashIter it) {
  return it.prev && it.prev->next ? HashEntry_value(it.prev->next) : NULL;
}
void* HashIter_value(HashIter it) {
  return !it.prev || !it.prev->next ? NULL
         : it.hash_table->key_size == it.hash_table->item_size
             ? HashEntry_value(it.prev->next)
             : (char*)HashEntry_value(it.prev->next) + it.hash_table->key_size;
}
HashIter HashIter_next(HashIter it) {
  if (it.prev && it.prev->next)
    it.prev = it.prev->next;
  return it;
}
bool HashIter_valid(HashIter it) { return it.prev && it.prev->next && it.hash_table; }
bool HashIter_equal(HashIter it1, HashIter it2) {
  if (it1.hash_table != it2.hash_table)
    return false;
  if (!it1.prev && !it2.prev)
    return true;
  return it1.prev && it2.prev && it1.prev->next == it2.prev->next;
}
