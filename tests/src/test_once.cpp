//
// Created by 14049 on 2024/7/25.
//

#include <gtest/gtest.h>
#include <xcl/concurrent/util/once.h>
#include <xcl/util/log.h>

#include <atomic>
#include <thread>

static std::atomic_bool b(false);

TEST(OnceTest, test1) {
  Once once = {0};
  b = false;
  auto func = []() {
    LOG_ASSERT_P(!b.exchange(true));
    ;
    return true;
  };
  std::thread threads[16];
  for (int i = 0; i < 16; i++)
    threads[i] = std::thread([&] { ASSERT_TRUE(Once_call(&once, func)); });
  for (int i = 0; i < 16; i++)
    threads[i].join();
}
TEST(OnceTest, test2) {
  Once once = {0};
  b = false;
  auto func = [](void* args) {
    std::atomic_bool* obj = (std::atomic_bool*)args;
    LOG_ASSERT_P(!obj->exchange(true));
    ;
    return true;
  };
  std::thread threads[16];
  for (int i = 0; i < 16; i++)
    threads[i] = std::thread([&] { ASSERT_TRUE(Once_call2(&once, func, &b)); });
  for (int i = 0; i < 16; i++)
    threads[i].join();
}
