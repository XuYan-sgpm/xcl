//
// Created by xuyan on 2022/6/24.
//

#include <gtest/gtest.h>
#include "concurrent/Lock.h"
#include <iostream>
using namespace std;

TEST(TestLock, func1) {
  xcl::Lock *lock = xcl::Lock::NewLock();
  lock->lock();
  lock->unlock();
  cout << "step1" << endl;
  lock->lock();
  lock->unlock();
  cout << "step2" << endl;
  lock->lock();
  cout << "step3" << endl;
  lock->lock();
  cout << "step4" << endl;
}