#include <gtest/gtest.h>
#include <string.h>
#include <xcl/util/algo.h>
#include <xcl/util/hash_map.h>

#include <iostream>
#include <set>
#include <unordered_map>

struct Pair {
  char key_[16];
  unsigned value_;

  Pair(unsigned value) { set(value); }

  Pair() : value_(0) { memset(key_, 0, sizeof(key_)); }

  void set(unsigned value) {
    memset(key_, 0, sizeof(key_));
    value_ = value;
    sprintf(key_, "%d", value);
  }
};

// static uint64_t __hash(const void* key)
//{
//     return hash(key, strlen((char*)key));
// }
// static bool __compare(const void* key1, const void* key2)
//{
//     return strcmp((const char*)key1, (const char*)key2) == 0;
// }

TEST(HashMapTest, func1) {
  void* map = HashMap_new(16, 4, nullptr, nullptr);
  for (int i = 0; i < 10; i++) {
    ASSERT_TRUE(HashMap_empty(map));
    auto st = std::chrono::steady_clock::now();
    ASSERT_NE(map, nullptr);
    Pair values[10240];
    int n = 10240;
    srand(time(nullptr));
    for (int i = 0; i < n; i++) {
      int val = i;
      values[i].set(val);
      ASSERT_TRUE(HashIter_valid(HashMap_add(map, values[i].key_, &val)));
    }
    ASSERT_EQ(HashMap_size(map), n);
    for (int i = 0; i < n; i++) {
      void* p = HashIter_value(HashMap_find(map, values[i].key_));
      ASSERT_NE(p, nullptr);
      ASSERT_EQ(*(unsigned*)p, values[i].value_);
      ASSERT_TRUE(HashMap_remove(map, values[i].key_));
      ASSERT_FALSE(HashIter_valid(HashMap_find(map, values[i].key_)));
      ASSERT_EQ(HashMap_size(map), n - i - 1);
    }
    ASSERT_TRUE(HashMap_empty(map));
    std::set<unsigned> s;
    for (int i = 0; i < n; i++) {
      int val = i;
      values[i].set(val);
      ASSERT_TRUE(HashIter_valid(HashMap_add(map, values[i].key_, &val)));
      ASSERT_TRUE(s.insert(val).second);
    }
    ASSERT_EQ(HashMap_size(map), n);
    for (auto it = HashMap_begin(map); HashIter_valid(it); it = HashIter_next(it)) {
      unsigned val = *(unsigned*)HashIter_value(it);
      ASSERT_TRUE(s.erase(val));
    }
    ASSERT_TRUE(s.empty());
    HashMap_clear(map);
    ASSERT_TRUE(HashMap_empty(map));
    for (int i = 0; i < n; i++)
      ASSERT_FALSE(HashIter_valid(HashMap_find(map, values[i].key_)));
    auto et = std::chrono::steady_clock::now();
    printf("hashmap test1 cost %lf ms\n",
           std::chrono::duration<double, std::milli>(et - st).count());
  }
  HashMap_free(map);
}
