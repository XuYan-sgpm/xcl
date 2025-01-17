//
// Created by 徐琰 on 2023/11/17.
//

#include <gtest/gtest.h>
#include <xcl/util/vector.h>

class VectorTest : public testing::Test {
 protected:
  Vector* vec_ = nullptr;

  void SetUp() override {
    Test::SetUp();
    vec_ = Vec_new(4);
    ASSERT_NE(vec_, nullptr);
    srand(time(nullptr));
  }
  void TearDown() override {
    Test::TearDown();
    Vec_free(vec_);
  }

  int getValue() { return rand(); }
};

TEST_F(VectorTest, func1) {
  int n = 10240;
  int values[10240];
  for (int i = 0; i < n; i++) {
    values[i] = getValue();
    ASSERT_TRUE(Vec_push(vec_, &values[i]));
    ASSERT_EQ(Vec_size(vec_), i + 1);
  }
  for (int i = 0; i < n; i++) {
    int val;
    ASSERT_TRUE(Vec_pop(vec_, &val));
    ASSERT_EQ(val, values[n - i - 1]);
    ASSERT_EQ(Vec_size(vec_), n - i - 1);
  }
  ASSERT_TRUE(Vec_empty(vec_));
}

TEST_F(VectorTest, func2) {
  int n = 10240;
  int values[10240];
  for (int i = 0; i < n; i++) {
    values[i] = getValue();
    ASSERT_TRUE(Vec_addn(vec_, 0, 1, &values[i]));
    ASSERT_EQ(Vec_size(vec_), i + 1);
  }
  for (int i = 0; i < n; i++) {
    int val = *(int*)Vec_pos(vec_, 0);
    ASSERT_TRUE(Vec_remove(vec_, 0, 1));
    ASSERT_EQ(Vec_size(vec_), n - i - 1);
    ASSERT_EQ(val, values[n - i - 1]);
  }
}

TEST_F(VectorTest, func3) {
  int n = 282;
  int* values = new int[n];
  for (int i = 0; i < n; i++) {
    int size = Vec_size(vec_);
    values[i] = getValue();
    ASSERT_TRUE(Vec_addn(vec_, 0, i + 1, &values[i]));
    ASSERT_EQ(Vec_size(vec_), size + i + 1);
  }
  ASSERT_EQ(Vec_size(vec_), n * (n + 1) / 2);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j <= i; j++) {
      int val;
      ASSERT_TRUE(Vec_pop(vec_, &val));
      ASSERT_EQ(values[i], val);
    }
  }
  ASSERT_TRUE(Vec_empty(vec_));
  delete[] values;
}

TEST_F(VectorTest, func4) {
  int n = 238476;
  int* values = new int[n];
  for (int i = 0; i < n; i++)
    values[i] = getValue();
  ASSERT_TRUE(Vec_add(vec_, 0, values, values + n));
  ASSERT_EQ(Vec_size(vec_), n);
  for (int i = 0; i < n; i++) {
    int val = *(int*)Vec_pos(vec_, i);
    ASSERT_EQ(val, values[i]);
  }
  Vec_clear(vec_);
  ASSERT_TRUE(Vec_empty(vec_));
  delete[] values;
}

TEST_F(VectorTest, func5) {
  int n = 8934;
  int* values = new int[n];
  for (int i = 0; i < n; i++) {
    values[i] = getValue();
  }
  ASSERT_TRUE(Vec_fill(vec_, values, values + n));
  for (int i = 0; i < n; i++) {
    int val = *(int*)Vec_pos(vec_, i);
    ASSERT_EQ(val, values[i]);
  }
  ASSERT_TRUE(Vec_remove(vec_, 0, Vec_size(vec_)));
  ASSERT_TRUE(Vec_empty(vec_));
  delete[] values;
}

TEST_F(VectorTest, func6) {
  int n = 34987;
  int val = getValue();
  ASSERT_TRUE(Vec_filln(vec_, n, &val));
  ASSERT_EQ(Vec_size(vec_), n);
  for (int i = 0; i < n; i++) {
    int val2 = *(int*)Vec_pos(vec_, i);
    ASSERT_EQ(val2, val);
  }
}
