//
// Created by 徐琰 on 2023/11/14.
//

#include <gtest/gtest.h>
#include <internal/util/hash_table.h>
#include <xcl/util/bytes.h>

TEST(HashTableTest, func1) {
  HashTable* hash_table = HashTable_new(4, 4, nullptr, nullptr, nullptr);
  ASSERT_NE(hash_table, nullptr);
  int values[81920];
  int n = 81920;
  srand(time(nullptr));
  for (int i = 0; i < n; i++) {
    int val = i;
    values[i] = val;
    ASSERT_TRUE(HashIter_valid(HashTable_add(hash_table, &val)));
  }
  ASSERT_EQ(HashTable_size(hash_table), n);
  for (int i = 0; i < n; i++) {
    void* p = HashIter_value(HashTable_find(hash_table, &values[i]));
    ASSERT_NE(p, nullptr);
    ASSERT_TRUE(*(int*)p == values[i]);
    ASSERT_TRUE(HashTable_remove(hash_table, &values[i]));
    ASSERT_FALSE(HashIter_valid(HashTable_find(hash_table, &values[i])));
    ASSERT_EQ(HashTable_size(hash_table), n - i - 1);
  }
  ASSERT_TRUE(HashTable_empty(hash_table));
  for (int i = 0; i < n; i++) {
    int val = i;
    values[i] = val;
    ASSERT_TRUE(HashIter_valid(HashTable_add(hash_table, &val)));
  }
  ASSERT_EQ(HashTable_size(hash_table), n);
  HashTable_clear(hash_table);
  ASSERT_TRUE(HashTable_empty(hash_table));
  for (int i = 0; i < n; i++) {
    ASSERT_FALSE(HashIter_valid(HashTable_find(hash_table, &values[i])));
  }
  HashTable_free(hash_table);
}
