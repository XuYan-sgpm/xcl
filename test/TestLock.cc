//
// Created by xuyan on 2022/6/24.
//

#include <gtest/gtest.h>
#include "concurrent/Lock.h"
#include <iostream>
#include <unistd.h>
using namespace std;

static void *__testLock(void *arg) {
  auto *lock = (xcl::TimedLock *)arg;
  for (;;) {
    auto st = chrono::steady_clock::now();
    if (!lock->tryLock(2000)) {
      auto et = chrono::steady_clock::now();
      auto duration = chrono::duration<double, milli>(et - st).count();
      cout << "acquire lock failed:" << duration << endl;
    } else {
      break;
    }
  }
  //  lock->lock();
  cout << "lock successfully" << endl;
  sleep(3);
  lock->unlock();
  cout << "unlock successfully" << endl;
  return nullptr;
}

TEST(TestLock, func1) {
  xcl::TimedLock *lock = xcl::TimedLock::NewLock();
  pthread_t threads[4];
  for (int i = 0; i < 4; i++) {
    ASSERT_EQ(pthread_create(&threads[i], nullptr, __testLock, lock), 0);
  }
  for (int i = 0; i < 4; i++) {
    pthread_join(threads[i], nullptr);
  }
}