//
// Created by xuyan on 2022/6/21.
//

#include <gtest/gtest.h>
#include "xcl/lang/CThreadLocal.h"
#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CThread.h"

#include <xcl/lang/system.h>
#include <iostream>
using namespace std;

static void
__testThreadLocal(CThreadLocal* local) {
  const char* p = "hello world";
  assert(Local_set(local, (void*)p));
  void* ptr;
  assert(Local_get(local, &ptr));
  assert(ptr == p);
  assert(Local_setInt32(local, 89));
  int32_t val;
  assert(Local_getInt32(local, &val));
  assert(val == 89);
  float val2;
  assert(Local_setFloat(local, 3.1415926f));
  assert(Local_getFloat(local, &val2));
  assert(val2 == 3.1415926f);
}

unsigned
threadProc(void* args) {
  CThreadLocal locals[10];
  for (int i = 0; i < 10; i++) {
    locals[i] = Local_make();
  }
  for (int loop = 0; loop < 10; loop++) {
    for (int i = 0; i < 10; i++) {
      __testThreadLocal(locals + i);
    }
    cout << "test local successfully" << endl;
    sleepMillis(200);
  }
  *(int*)args = 0;
  return 0;
}

TEST(ThreadLocal, func1) {
  int n = 4;
  CThread* threads[n];
  int code[n];
  for (int i = 0; i < n; i++) {
    code[i] = -1;
  }
  for (int i = 0; i < n; i++) {
    threads[i] = Thread_new(false, threadProc, code + i);
    ASSERT_NE(threads[i], nullptr);
  }
  for (int i = 0; i < n; i++) {
    Thread_join(threads[i]);
  }
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(code[0], 0);
  }
}