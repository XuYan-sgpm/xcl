//
// Created by xuyan on 2022/6/21.
//

#include <gtest/gtest.h>
#include <lang/CThreadLocal.h>
#include <lang/CLocalStorage.h>

#include <iostream>
#include <zconf.h>
using namespace std;

static void __testThreadLocal(CThreadLocal* local) {
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

void* threadProc(void* args) {
  CThreadLocal locals[10];
  for (int i = 0; i < 10; i++) {
    locals[i] = Local_make();
  }
  for (int loop = 0; loop < 10; loop++) {
    for (int i = 0; i < 10; i++) {
      __testThreadLocal(locals + i);
    }
    cout << "test local successfully" << endl;
    sleep(0.2);
  }
  *(int*)args = 0;
  return args;
}

TEST(ThreadLocal, func1) {
  int n = 4;
  pthread_t threads[n];
  int code[n];
  for (int i = 0; i < n; i++) {
    code[i] = -1;
  }
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(pthread_create(&threads[i], nullptr, threadProc, code + i), 0);
  }
  for (int i = 0; i < n; i++) {
    pthread_join(threads[i], nullptr);
  }
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(code[0], 0);
  }
}