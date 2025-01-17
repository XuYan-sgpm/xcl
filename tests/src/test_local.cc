//
// Created by 徐琰 on 2023/10/17.
//

#include <gtest/gtest.h>
#include <stdio.h>
#include <xcl/lang/thread_local.h>
#include <xcl/util/log.h>

#include <thread>

static void __destructor(void* args) { LOG_P(LOG_ERROR, "destructor"); }

class LocalTest : public testing::Test {
 protected:
  static void SetUpTestCase() {}
};

TEST_F(LocalTest, func1) {
  int id = Local_new();
  printf("local id:%d\n", id);
  ASSERT_GE(id, 0);
  void* p = (void*)0xff;
  auto func = [&] {
    for (int i = 0; i < 100; i++) {
      ASSERT_TRUE(Local_set(id, p, __destructor));
      ASSERT_EQ(Local_get(id), p);
    }
  };

  std::thread threads[16];
  for (int i = 0; i < 1; i++)
    threads[i] = std::thread(func);
  for (int i = 0; i < 1; i++)
    threads[i].join();
}
