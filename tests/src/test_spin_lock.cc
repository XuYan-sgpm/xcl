//
// Created by 徐琰 on 2023/10/21.
//

#include <gtest/gtest.h>
#include <xcl/concurrent/util/spin_lock.h>

#include <thread>

TEST(SpinLockTest, func1) {
  SpinLock spin_lock = {0};
  int size = 0;
  int per_thread = 100000;
  auto func = [&] {
    for (int i = 0; i < per_thread; i++) {
      SpinLock_lock(&spin_lock);
      ++size;
      SpinLock_unlock(&spin_lock);
    }
  };
  int thread_count = 8;
  std::thread* threads = new std::thread[thread_count];
  for (int i = 0; i < thread_count; i++)
    threads[i] = std::thread(func);
  for (int i = 0; i < thread_count; i++)
    threads[i].join();
  ASSERT_EQ(size, thread_count * per_thread);
  delete[] threads;
}
