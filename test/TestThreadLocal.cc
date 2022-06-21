//
// Created by xuyan on 2022/6/21.
//

#include <gtest/gtest.h>
#include <lang/CThreadLocal.h>
#include <lang/CLocalStorage.h>

#include <iostream>
using namespace std;
#include <mutex>
#include <zconf.h>

static mutex m;

static void safePrint(){
  lock_guard<mutex> guard(m);
  debug();
}

void *threadProc(void *args) {
  safePrint();
  sleep(100000);
  return NULL;
}

TEST(ThreadLocal, func1) {
  int integer = 89;
  setLocal(&integer);
  ASSERT_EQ(*(int *)getLocal(), 89);
  pthread_t threads[5];
  for (int i = 0; i < 5; i++) {
    pthread_create(&threads[i], NULL, threadProc, NULL);
  }
  for (int i = 0; i < 5; i++) {
    pthread_join(threads[i], NULL);
  }
}