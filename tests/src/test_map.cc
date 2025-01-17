//
// Created by 14049 on 2024/6/24.
//

#include <gtest/gtest.h>
#include <xcl/util/map.h>

struct IntComp : public Comparator {
  static int compare(Comparator* comparator, const void* obj1, const void* obj2) {
    return *(int*)obj1 - *(int*)obj2;
  }
  IntComp() {
    static void* __vptr[] = {(void*)compare};
    this->__vptr = (decltype(this->__vptr))__vptr;
  }
};

class MapTest : public testing::Test {
 protected:
  Map* map;
  IntComp comp;

  void SetUp() override {
    map = Map_new(sizeof(int), sizeof(int), &comp);
    ASSERT_NE(map, nullptr);
  }
  void TearDown() override { Map_free(map); }
  void add(bool unique, int key, int val) {
    ASSERT_TRUE(TreeIter_valid(Map_add(map, unique, &key, &val)));
    Map_verify(map);
  }
  void remove(int key) {
    TreeIter iter = Map_find(map, &key);
    ASSERT_FALSE(TreeIter_equal(Map_end(map), iter));
    Map_removeIter(map, iter);
    Map_verify(map);
  }
};

TEST_F(MapTest, test1) {
  for (int i = 0; i < 4096; i++)
    add(true, i, i);
  for (int i = 0; i < 4096; i++)
    remove((i + 2397) % 4096);
  ASSERT_TRUE(Map_empty(map));
}

TEST_F(MapTest, test2) {
  for (int i = 0; i < 10240; i++)
    add(true, i, i);
  Map* map2 = Map_copy(map);
  ASSERT_NE(map2, nullptr);
  ASSERT_TRUE(Map_equal(map, map2));
  Map_free(map2);
}

TEST_F(MapTest, test3) {
  for (int i = 0; i < 10240; i++)
    add(true, i, i);
  TreeIter iter = Map_begin(map);
  for (int i = 0; !TreeIter_equal(iter, Map_end(map)); iter = TreeIter_next(iter), i++) {
    int val = *(int*)TreeIter_value(iter);
    ASSERT_EQ(i, val);
    ASSERT_EQ(*(int*)Map_get(map, &i), val);
  }
}

TEST_F(MapTest, test4) {
  int key = 874;
  for (int i = 0; i < 10; i++)
    add(false, key, 8724);
  for (int i = 0; i < 1234; i++)
    add(false, i + 900, 3298);
  TreeIter range[2];
  Map_range(map, &key, range);
  int count = 0;
  TreeIter iter = range[0];
  for (; !TreeIter_equal(iter, range[1]); iter = TreeIter_next(iter), ++count)
    ASSERT_EQ(*(int*)TreeIter_key(iter), key);
  ASSERT_EQ(count, 10);
}

TEST_F(MapTest, test5) {
  for (int i = 0; i < 50; i++)
    add(true, i, i);
  Map* map2 = Map_new(4, 4, &comp);
  ASSERT_NE(map2, nullptr);
  for (int i = 100; i < 200; i++)
    ASSERT_TRUE(TreeIter_valid(Map_add(map2, true, &i, &i)));
  Map_swap(map, map2);
  TreeIter iter = Map_begin(map);
  for (int i = 0; i < 100; i++) {
    ASSERT_FALSE(TreeIter_equal(iter, Map_end(map)));
    ASSERT_EQ(i + 100, *(int*)TreeIter_key(iter));
    iter = TreeIter_next(iter);
  }
  iter = Map_begin(map2);
  for (int i = 0; i < 50; i++) {
    ASSERT_FALSE(TreeIter_equal(iter, Map_end(map2)));
    ASSERT_EQ(i, *(int*)TreeIter_key(iter));
    iter = TreeIter_next(iter);
  }
  Map_free(map2);
}
