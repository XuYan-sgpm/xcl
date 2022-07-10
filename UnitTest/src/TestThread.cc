//
// Created by xuyan on 2022/7/10.
//

#include <gtest/gtest.h>
#include <xcl/lang/CThread.h>
#include <windows.h>
#include <synchapi.h>
#include <iostream>
using namespace std;

static CThread* t = NULL;

unsigned __stdcall __threadProc(void*) {
  Sleep(3000);
  cout << "sleep finished" << endl;
  CThread* currentThread = Thread_current();
  assert(Thread_delete(currentThread) == false);
  return 0;
}

static void
__testThreadSuspend(void*) {
  CThread* thread = Thread_current();
}

TEST(TestThread, func1) {
  CThread* thread = Thread_new(true, __threadProc, NULL);
  Thread_addCbFront(thread, __testThreadSuspend, NULL);
  t = thread;
  Thread_start(thread);
  Thread_join(thread);
  assert(Thread_delete(thread));
}