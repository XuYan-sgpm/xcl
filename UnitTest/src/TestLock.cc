//
// Created by xuyan on 2022/6/24.
//

#include <gtest/gtest.h>
#include "xcl/concurrent/Lock.h"
#include "xcl/concurrent/CMutex.h"
#include <iostream>
#include "xcl/lang/system.h"
#include "xcl/lang/CThread.h"
using namespace std;

static void
__testLock(void* args) {
  auto* lock = (xcl::TimedLock*)args;
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
  sleepMillis(3000);
  lock->unlock();
  cout << "unlock successfully" << endl;
}

static void
__testCMutex(void* args) {
  for (;;) {
    auto st = chrono::steady_clock::now();
    //    auto st = nanos();
    if (!Mutex_tryLock2(args, 20)) {
      auto et = chrono::steady_clock::now();
      //      auto et = nanos();
      auto duration = chrono::duration<double, milli>(et - st).count();
      //      auto duration = (et - st) / 1000000;
      cout << "acquire lock failed:" << duration << endl;
    } else {
      break;
    }
  }
  //  lock->lock();
  cout << "lock successfully" << endl;
  sleepMillis(40);
  Mutex_unlock(args);
  cout << "unlock successfully" << endl;
}

static void
__runLockThreads(Callback cb, void* args) {
  int32_t nThreads = 8;
  CThread* threads[nThreads];
  for (int i = 0; i < nThreads; i++) {
    threads[i] = Thread_new(false, cb, args);
    ASSERT_NE(threads[i], nullptr);
  }
  for (int i = 0; i < nThreads; i++) {
    Thread_join(threads[i]);
  }
}

// TEST(TestLock, func1) {
//   xcl::TimedLock* lock = xcl::TimedLock::NewLock();
//   __runLockThreads(__testLock, lock);
// }

TEST(TestLock, func2) {
  void* mutex = Mutex_new();
  assert(mutex != NULL);
  __runLockThreads(__testCMutex, mutex);
  Mutex_delete(mutex);
}