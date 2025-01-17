//
// Created by 14049 on 2024/6/27.
//

#include <gtest/gtest.h>
#include <xcl/util/handle.h>
#include <xcl/util/log.h>

class HandleTest : public testing::Test {
 protected:
  void (*destructor)(void*);
  int* values;
  int n;

  void SetUp() override {
    n = 1000000;
    values = new int[n];
    destructor = [](void* ptr) {
      // LOG_P( LOG_DEBUG, "free pointer valued %d", *(int*)ptr);
    };
    for (int i = 0; i < n; i++)
      values[i] = i;
  }

  void TearDown() override { delete[] values; }
};

TEST_F(HandleTest, test1) {
  void** handles = new void*[n];
  for (int i = 0; i < n; i++) {
    std::string name = std::to_string(values[i]);
    handles[i] = Handle_new(&values[i], destructor, name.c_str());
    ASSERT_NE(handles[i], nullptr);
    void* p = Handle_ref(handles[i]);
    ASSERT_NE(p, nullptr);
    ASSERT_EQ(*(int*)Handle_value(handles[i]), values[i]);
    ASSERT_EQ(*(int*)Handle_value(p), values[i]);
    Handle_close(p);
    ASSERT_EQ(*(int*)Handle_value(handles[i]), values[i]);
    ASSERT_EQ(Handle_value(p), nullptr);
    void* q = Handle_refName(name.c_str());
    ASSERT_NE(q, nullptr);
    ASSERT_EQ(*(int*)Handle_value(q), values[i]);
    Handle_close(handles[i]);
    ASSERT_EQ(Handle_value(handles[i]), nullptr);
    ASSERT_EQ(*(int*)Handle_value(q), values[i]);
    Handle_close(q);
    ASSERT_EQ(Handle_value(q), nullptr);
    ASSERT_EQ(Handle_refName(name.c_str()), nullptr);
  }
  delete[] handles;
}

TEST_F(HandleTest, test2) {
  void** handles = new void*[n];
  for (int i = 0; i < n; i++) {
    std::string name = std::to_string(values[i]);
    handles[i] = Handle_new(&values[i], destructor, name.c_str());
    ASSERT_NE(handles[i], nullptr);
    void* p = Handle_ref(handles[i]);
    ASSERT_NE(p, nullptr);
    ASSERT_EQ(*(int*)Handle_value(handles[i]), values[i]);
    ASSERT_EQ(*(int*)Handle_value(p), values[i]);
    ASSERT_TRUE(Handle_hasName(name.c_str()));
    Handle_close(handles[i]);
    Handle_close(p);
    ASSERT_EQ(Handle_value(handles[i]), nullptr);
    ASSERT_EQ(Handle_value(p), nullptr);
    ASSERT_FALSE(Handle_hasName(name.c_str()));
  }
  delete[] handles;
}

TEST_F(HandleTest, test3) {
  void** handles = new void*[n];
  int val = 8765234;
  std::string name = std::to_string(val);
  handles[0] = Handle_new(&val, destructor, name.c_str());
  ASSERT_EQ(*(int*)Handle_value(handles[0]), val);
  for (int i = 1; i < n; i++) {
    handles[i] = Handle_ref(handles[0]);
    ASSERT_EQ(*(int*)Handle_value(handles[i]), val);
  }
  Handle_closeName(name.c_str());
  ASSERT_FALSE(Handle_hasName(name.c_str()));
  for (int i = 0; i < n; i++)
    ASSERT_EQ(Handle_value(handles[i]), nullptr);
  delete[] handles;
}

TEST_F(HandleTest, test4) {
  n >>= 2;
  void** handles = new void*[n];
  char name[32];
  for (int i = 0; i < n; i++) {
    snprintf(name, sizeof(name), "%d", values[i]);
    handles[i] = Handle_new(&values[i], destructor, name);
    ASSERT_EQ(*(int*)Handle_value(handles[i]), values[i]);
  }
  Handle_closeAll();
  for (int i = 0; i < n; i++)
    ASSERT_EQ(Handle_value(handles[i]), nullptr);
  delete[] handles;
}

TEST_F(HandleTest, test5) {
  void** handles = new void*[n];
  void* h;
  for (int i = 0; i < n; i++) {
    std::string name = std::to_string(values[i]);
    handles[i] = Handle_new(&values[i], destructor, name.c_str());
    ASSERT_NE(handles[i], nullptr);
    h = handles[i];
    Handle_close(handles[i]);
    ASSERT_EQ(Handle_value(handles[i]), nullptr);
    handles[i] = Handle_new(&values[i], destructor, name.c_str());
    ASSERT_NE(handles[i], nullptr);
    ASSERT_NE(handles[i], h);
    ASSERT_EQ(*(int*)Handle_value(handles[i]), values[i]);
    Handle_close(handles[i]);
  }
  delete[] handles;
}

TEST_F(HandleTest, test6) {
  void** handles = new void*[n];
  char name[32];
  for (int i = 0; i < n; i++) {
    snprintf(name, sizeof(name), "%d", i);
    handles[i] = Handle_new(&values[i], destructor, "");
    ASSERT_NE(handles[i], nullptr);
    ASSERT_FALSE(Handle_hasName(name));
  }
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(*(int*)Handle_value(handles[i]), values[i]);
    Handle_close(handles[i]);
    ASSERT_EQ(Handle_value(handles[i]), nullptr);
  }
  delete[] handles;
}
