//
// Created by xuyan on 2022/6/24.
//

#include <gtest/gtest.h>
#include "xcl/concurrent/Lock.h"
#include "xcl/concurrent/CMutex.h"
#include <iostream>
#include <Windows.h>
#include "xcl/lang/system.h"
#include "xcl/lang/CThread.h"
using namespace std;

static unsigned
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
  return 0;
}

static unsigned __stdcall __testCMutex(void* args) {
  for (;;) {
    auto st = chrono::steady_clock::now();
    if (!Mutex_tryLock2(args, 20)) {
      auto et = chrono::steady_clock::now();
      auto duration = chrono::duration<double, milli>(et - st).count();
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
  return 0;
}

static void __stdcall __runLockThreads(ThreadProc threadProc, void* args) {
  int32_t nThreads = 8;
  CThread* threads[nThreads];
  for (int i = 0; i < nThreads; i++) {
    threads[i] = Thread_new(false, threadProc, args);
    ASSERT_NE(threads[i], nullptr);
  }
  for (int i = 0; i < nThreads; i++) {
    Thread_join(threads[i]);
  }
}

TEST(TestLock, func1) {
  xcl::TimedLock* lock = xcl::TimedLock::NewLock();
  __runLockThreads(__testLock, lock);
}

TEST(TestLock, func2) {
  void* mutex = Mutex_new();
  assert(mutex != NULL);
  __runLockThreads(__testCMutex, mutex);
  Mutex_delete(mutex);
}