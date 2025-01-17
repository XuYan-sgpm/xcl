//
// Created by 14049 on 2023/12/1.
//

#include <gtest/gtest.h>
#include <xcl/util/heap.h>

#include <iostream>

static int compareInteger(Comparator* obj, const void* p, const void* q) {
  return *(const int*)p - *(const int*)q;
}
static Comparator intComp() {
  static void* int_comp_vptr[] = {(void*)compareInteger};
  Comparator obj;
  obj.__vptr = (decltype(obj.__vptr))int_comp_vptr;
  return obj;
}

class HeapTest : public ::testing::Test {
  int rand_val_ = 0;

 protected:
  void SetUp() override {}

  int getVal() {
    srand(time(nullptr) + rand_val_);
    return (rand_val_ = rand());
  }
};

TEST_F(HeapTest, func1) {
  auto comp = intComp();
  Heap* h = Heap_new(sizeof(int), &comp);
  ASSERT_NE(h, nullptr);
  int n = 81920;
  for (int i = 0; i < n; i++) {
    int val = getVal();
    ASSERT_TRUE(Heap_push(h, &val));
    ASSERT_EQ(Heap_size(h), i + 1);
  }
  int prev;
  ASSERT_TRUE(Heap_pop(h, &prev));
  for (int i = 1; i < n; i++) {
    ASSERT_EQ(Heap_size(h), n - i);
    int val;
    ASSERT_TRUE(Heap_pop(h, &val));
    ASSERT_GE(val, prev);
    prev = val;
  }
  ASSERT_TRUE(Heap_empty(h));
  Heap_free(h);
}
