//
// Created by 徐琰 on 2023/11/14.
//

#include <internal/util/hash_table.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/define.h>
#include <xcl/lang/system.h>
#include <xcl/util/algo.h>
#include <xcl/util/log.h>

HashIter HashIter_make(HashTable* hash_table, HashEntry* prev);
static inline uint64_t HashTable_hash(HashTable* hash_table, const void* key) {
  return hash_table->key_hasher ? hash_table->key_hasher->__vptr->hash(hash_table->key_hasher, key)
                                : hash(key, hash_table->key_size);
}
static inline bool HashTable_compare(HashTable* hash_table, const void* key1, const void* key2) {
  return (hash_table->key_comp
              ? hash_table->key_comp->__vptr->compare(hash_table->key_comp, key1, key2)
              : memcmp(key1, key2, hash_table->key_size)) == 0;
}
static inline const void* HashTable_key(HashTable* hash_table, const void* o) {
  return hash_table->key_handler ? hash_table->key_handler(o) : o;
}
static inline unsigned HashTable_bucket(HashTable* hash_table, HashEntry* entry) {
  return entry->hash_value & (hash_table->cap - 1);
}

static HashEntry* HashEntry_alloc(unsigned item_size) {
  HashEntry* e = palloc(sizeof(HashEntry) + item_size);
  if (e)
    e->next = NULL;
  return e;
}
static inline always_inline void HashEntry_dealloc(HashEntry* e) { pfree(e); }

void HashTable_init(HashTable* hash_table, unsigned key_size, unsigned item_size,
                    struct Hasher* key_hasher, struct Comparator* key_comp,
                    const void* (*key_handler)(const void*)) {
  memset(hash_table, 0, sizeof(*hash_table));
  static const unsigned initial_hash_table_cap = 8;
  LOG_ASSERT_P((hash_table->buckets = palloc(sizeof(void*) * initial_hash_table_cap)));
  LOG_ASSERT_P((hash_table->header = HashEntry_alloc(item_size)));
  hash_table->item_size = item_size;
  hash_table->key_size = key_size;
  hash_table->cap = initial_hash_table_cap;
  memset(hash_table->buckets, 0, sizeof(void*) * initial_hash_table_cap);
  hash_table->key_hasher = key_hasher;
  hash_table->key_comp = key_comp;
  hash_table->key_handler = key_handler;
}

HashTable* HashTable_new(unsigned key_size, unsigned item_size, struct Hasher* key_hasher,
                         struct Comparator* key_comp, const void* (*key_handler)(const void*)) {
  HashTable* hash_table = palloc(sizeof(HashTable));
  if (hash_table)
    HashTable_init(hash_table, key_size, item_size, key_hasher, key_comp, key_handler);
  return hash_table;
}

void HashTable_destroy(HashTable* hash_table) {
  HashTable_clear(hash_table);
  HashEntry_dealloc(hash_table->header);
  pfree(hash_table->buckets);
  memset(hash_table, 0, sizeof(*hash_table));
}

void HashTable_free(HashTable* hash_table) {
  if (hash_table) {
    HashTable_clear(hash_table);
    HashEntry_dealloc(hash_table->header);
    pfree(hash_table->buckets);
    pfree(hash_table);
  }
}

static HashEntry* HashTable_push(HashTable* hash_table, HashEntry* e) {
  unsigned bucket_id = e->hash_value & (hash_table->cap - 1);
  HashEntry* top = hash_table->buckets[bucket_id];
  if (top) {
    e->next = top->next;
    top->next = e;
  } else {
    e->next = hash_table->header->next;
    hash_table->header->next = e;
    hash_table->buckets[bucket_id] = hash_table->header;
    if (e->next)
      hash_table->buckets[HashTable_bucket(hash_table, e->next)] = e;
  }
  ++hash_table->size;
  HashEntry* prev = hash_table->buckets[bucket_id];
  LOG_ASSERT_P(prev->next == e);
  return prev;
}

static bool HashTable_rehash(HashTable* hash_table) {
  HashEntry* e = hash_table->header->next;
  hash_table->header->next = NULL;
  HashTable new_table = *hash_table;
  new_table.cap <<= 1;
  LOG_ASSERT_P((new_table.buckets = palloc(new_table.cap * sizeof(void*))));
  memset(new_table.buckets, 0, sizeof(void*) * new_table.cap);
  new_table.size = 0;
  while (e) {
    void* next = e->next;
    HashTable_push(&new_table, e);
    --hash_table->size;
    e = next;
  }
  LOG_ASSERT_PMSG(!hash_table->size,

                  "hash table remaining %d", hash_table->size);
  pfree(hash_table->buckets);
  *hash_table = new_table;
  return true;
}

HashIter HashTable_reserve(HashTable* hash_table, bool unique, const void* key) {
  if (unique && HashIter_valid(HashTable_find(hash_table, key)))
    return HashTable_end(hash_table);
  if (hash_table->size >= (hash_table->cap >> 2) * 3 && !HashTable_rehash(hash_table))
    return HashTable_end(hash_table);
  HashEntry* e = NULL;
  if (!(e = HashEntry_alloc(hash_table->item_size)))
    return HashTable_end(hash_table);
  e->hash_value = HashTable_hash(hash_table, key);
  return HashIter_make(hash_table, HashTable_push(hash_table, e));
}

HashIter HashTable_add(HashTable* hash_table, const void* item) {
  HashIter it = HashTable_reserve(hash_table, false, HashTable_key(hash_table, item));
  if (HashIter_valid(it))
    memcpy(HashEntry_value(it.prev->next), item, hash_table->item_size);
  return it;
}

HashIter HashTable_addUnique(HashTable* hash_table, const void* item) {
  HashIter it = HashTable_reserve(hash_table, true, HashTable_key(hash_table, item));
  if (HashIter_valid(it))
    memcpy(HashEntry_value(it.prev->next), item, hash_table->item_size);
  return it;
}

bool HashTable_remove(HashTable* hash_table, const void* key) {
  HashIter it = HashTable_find(hash_table, key);
  return HashIter_valid(it) ? (HashTable_removeAt(hash_table, it), true) : false;
}

bool HashTable_removeAt(HashTable* hash_table, HashIter it) {
  if (HashIter_valid(it) && it.hash_table == hash_table) {
    HashEntry* prev = it.prev;
    HashEntry* e = prev->next;
    unsigned bucket_id = HashTable_bucket(hash_table, e);
    // e 所在的bucket没有item了
    if ((prev == hash_table->header || HashTable_bucket(hash_table, prev) != bucket_id) &&
        (!e->next || HashTable_bucket(hash_table, e->next) != bucket_id))
      hash_table->buckets[bucket_id] = NULL;
    // e 是 e->next 所在bucket的第一个元素
    if (e->next && HashTable_bucket(hash_table, e->next) != bucket_id)
      hash_table->buckets[HashTable_bucket(hash_table, e->next)] = prev;
    prev->next = e->next;
    --hash_table->size;
    HashEntry_dealloc(e);
    return true;
  }
  return false;
}

HashIter HashTable_find(HashTable* hash_table, const void* key) {
  unsigned bucket_id = HashTable_hash(hash_table, key) & (hash_table->cap - 1);
  HashEntry* prev = hash_table->buckets[bucket_id];
  if (!prev)
    return HashTable_end(hash_table);
  for (HashEntry* e = prev->next; e && bucket_id == HashTable_bucket(hash_table, e);
       prev = e, e = e->next) {
    if (HashTable_compare(hash_table, key, HashTable_key(hash_table, HashEntry_value(e))))
      return HashIter_make(hash_table, prev);
  }
  return HashTable_end(hash_table);
}

void HashTable_clear(HashTable* hash_table) {
  HashEntry* e = hash_table->header->next;
  while (e) {
    void* next = e->next;
    HashEntry_dealloc(e);
    e = next;
    --hash_table->size;
  }
  LOG_ASSERT_P(!hash_table->size);
  hash_table->header->next = NULL;
  memset(hash_table->buckets, 0, sizeof(void*) * hash_table->cap);
}

HashIter HashTable_begin(HashTable* hash_table) {
  return HashIter_make(hash_table, hash_table->header);
}

HashIter HashTable_end(HashTable* hash_table) {
  static HashEntry nil = {0};
  return HashIter_make(hash_table, &nil);
}
