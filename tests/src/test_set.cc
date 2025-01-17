//
// Created by 14049 on 2024/6/24.
//
#include <gtest/gtest.h>
#include <xcl/util/set.h>

struct IntComp : public Comparator {
  static int compare(Comparator* comparator, const void* obj1, const void* obj2) {
    return *(int*)obj1 - *(int*)obj2;
  }
  IntComp() {
    static void* __vptr[] = {(void*)compare};
    this->__vptr = (decltype(this->__vptr))__vptr;
  }
};

class SetTest : public testing::Test {
 protected:
  Set* set;
  IntComp comp;

  void SetUp() override {
    set = Set_new(sizeof(int), &comp);
    ASSERT_NE(set, nullptr);
  }
  void TearDown() override { Set_free(set); }
  void add(bool unique, int val) {
    ASSERT_TRUE(TreeIter_valid(Set_add(set, unique, &val)));
    Set_verify(set);
  }
  void remove(int key) {
    TreeIter iter = Set_find(set, &key);
    ASSERT_FALSE(TreeIter_equal(Set_end(set), iter));
    Set_removeIter(set, iter);
    Set_verify(set);
  }
};

TEST_F(SetTest, test1) {
  for (int i = 0; i < 4096; i++)
    add(true, i);
  for (int i = 0; i < 4096; i++)
    remove((i + 2397) % 4096);
  ASSERT_TRUE(Set_empty(set));
}

TEST_F(SetTest, test2) {
  for (int i = 0; i < 10240; i++)
    add(true, i);
  Set* map2 = Set_copy(set);
  ASSERT_NE(map2, nullptr);
  ASSERT_TRUE(Set_equal(set, map2));
  Set_free(map2);
}

TEST_F(SetTest, test3) {
  for (int i = 0; i < 10240; i++)
    add(true, i);
  TreeIter iter = Set_begin(set);
  for (int i = 0; !TreeIter_equal(iter, Set_end(set)); iter = TreeIter_next(iter), i++) {
    int val = *(int*)TreeIter_value(iter);
    ASSERT_EQ(i, val);
  }
}

TEST_F(SetTest, test4) {
  int key = 874;
  for (int i = 0; i < 10; i++)
    add(false, key);
  for (int i = 0; i < 1234; i++)
    add(false, i + 900);
  TreeIter range[2];
  Set_range(set, &key, range);
  int count = 0;
  TreeIter iter = range[0];
  for (; !TreeIter_equal(iter, range[1]); iter = TreeIter_next(iter), ++count)
    ASSERT_EQ(*(int*)TreeIter_value(iter), key);
  ASSERT_EQ(count, 10);
}

TEST_F(SetTest, test5) {
  for (int i = 0; i < 50; i++)
    add(true, i);
  Set* set2 = Set_new(4, &comp);
  ASSERT_NE(set2, nullptr);
  for (int i = 100; i < 200; i++)
    ASSERT_TRUE(TreeIter_valid(Set_add(set2, true, &i)));
  Set_swap(set, set2);
  TreeIter iter = Set_begin(set);
  for (int i = 0; i < 100; i++) {
    ASSERT_FALSE(TreeIter_equal(iter, Set_end(set)));
    ASSERT_EQ(i + 100, *(int*)TreeIter_value(iter));
    iter = TreeIter_next(iter);
  }
  iter = Set_begin(set2);
  for (int i = 0; i < 50; i++) {
    ASSERT_FALSE(TreeIter_equal(iter, Set_end(set2)));
    ASSERT_EQ(i, *(int*)TreeIter_value(iter));
    iter = TreeIter_next(iter);
  }
  Set_free(set2);
}
