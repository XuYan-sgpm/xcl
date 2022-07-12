//
// Created by xuyan on 2022/7/10.
//

#include <gtest/gtest.h>
#include <xcl/lang/CThread.h>
#include <iostream>
#include "xcl/lang/system.h"
using namespace std;

static CThread* t = NULL;

void XCL_API
__threadProc(void*) {
  cout << "enter thread" << endl;
  sleepMillis(3000);
  cout << "sleep finished" << endl;
  CThread* currentThread = Thread_current();
  assert(Thread_delete(currentThread) == false);
}

static void
__testThreadSuspend(void*) {
  CThread* thread = Thread_current();
}

TEST(TestThread, func1) {
  CThread* thread = Thread_new(true, __threadProc, NULL);
  Thread_addCbFront(thread, __testThreadSuspend, NULL);
  t = thread;
  cout << "before main sleep" << endl;
  sleepMillis(1000);
  cout << "main sleep after" << endl;
  Thread_start(thread);
  Thread_join(thread);
  assert(Thread_delete(thread));
}