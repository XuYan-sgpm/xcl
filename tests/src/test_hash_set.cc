//
// Created by 徐琰 on 2023/11/14.
//

#include <gtest/gtest.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/hash_set.h>
#include <xcl/util/log.h>

#include <chrono>

// static uint64_t __Int_hash(const void* a)
//{
//     return *(const int*)a;
// }
// static bool __Int_cmp(const void* x, const void* y)
//{
//     return *(const int*)x == *(const int*)y;
// }

TEST(HashSetTest, func1) {
  void* set = HashSet_new(4, nullptr, nullptr);
  for (int i = 0; i < 10; i++) {
    auto st = std::chrono::steady_clock::now();
    ASSERT_NE(set, nullptr);
    int values[81920];
    int n = 81920;
    srand(time(nullptr));
    for (int i = 0; i < n; i++) {
      values[i] = i;
      ASSERT_TRUE(HashIter_valid(HashSet_add(set, &values[i])));
    }
    ASSERT_EQ(HashSet_size(set), n);
    unsigned size = 0;
    auto iter = HashSet_begin(set);
    while (!HashIter_equal(iter, HashSet_end(set))) {
      ++size;
      iter = HashIter_next(iter);
    }
    ASSERT_EQ(size, n);
    for (int i = 0; i < n; i++) {
      void* p = HashIter_value(HashSet_find(set, &values[i]));
      ASSERT_NE(p, nullptr);
      ASSERT_TRUE(memcmp(p, &values[i], 4) == 0);
      ASSERT_TRUE(HashSet_remove(set, &values[i]));
      ASSERT_FALSE(HashIter_valid(HashSet_find(set, &values[i])));
      ASSERT_EQ(HashSet_size(set), n - i - 1);
    }
    ASSERT_TRUE(HashSet_empty(set));
    for (int i = 0; i < n; i++) {
      values[i] = i;
      ASSERT_TRUE(HashIter_valid(HashSet_add(set, &values[i])));
    }
    ASSERT_EQ(HashSet_size(set), n);
    HashSet_clear(set);
    ASSERT_TRUE(HashSet_empty(set));
    for (int i = 0; i < n; i++) {
      ASSERT_FALSE(HashIter_valid(HashSet_find(set, &values[i])));
    }
    auto et = std::chrono::steady_clock::now();
    printf("hashset test1 cost %lf ms\n",
           std::chrono::duration<double, std::milli>(et - st).count());
  }
  HashSet_free(set);
}
