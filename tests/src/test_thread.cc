//
// Created by 徐琰 on 2023/10/13.
//
#include <gtest/gtest.h>
#include <xcl/lang/system.h>
#include <xcl/lang/thread.h>
#include <xcl/util/log.h>

#include <iostream>
#include <thread>

static Runnable* exitHandler();
static Runnable* injectHandler();
static void _run(Runnable* args) {
  LOG_P(LOG_DEBUG, "enter thread routine");
  ASSERT_TRUE(Thread_atExit(exitHandler()));
  ASSERT_TRUE(Thread_atExit(injectHandler()));
  msleep(1000);
}
static void _run2(Runnable* runnable) { LOG_P(LOG_DEBUG, "exit notify"); }
static void _run3(Runnable* runnable) { LOG_P(LOG_DEBUG, "线程注入"); }

static Runnable* runnable() {
  static Runnable runnable = {0};
  static void* clazz[] = {(void*)_run};
  runnable.__vptr = (decltype(runnable.__vptr))clazz;
  return &runnable;
}
static Runnable* exitHandler() {
  static Runnable r = {0};
  static void* clazz[] = {(void*)_run2};
  r.__vptr = (decltype(r.__vptr))clazz;
  return &r;
}
static Runnable* injectHandler() {
  static Runnable r = {0};
  static void* clazz[] = {(void*)_run3};
  r.__vptr = (decltype(r.__vptr))clazz;
  return &r;
}

#define THREAD_COUNT 8

TEST(ThreadTest, func1) {
  uintptr_t threads[THREAD_COUNT] = {0};
  for (int i = 0; i < THREAD_COUNT; i++) {
    threads[i] = Thread_create(runnable());
  }
  for (int i = 0; i < THREAD_COUNT; i++) {
    ASSERT_TRUE(Thread_join(threads[i]));
  }
  std::cout << "threads_ test finished once" << std::endl;
  std::thread t([]() -> void {
    LOG_P(LOG_DEBUG, "normal run");
    ASSERT_TRUE(Thread_atExit(injectHandler()));
    LOG_P(LOG_DEBUG, "normal exit");
  });
  auto h = t.native_handle();
  t.join();
  LOG_P(LOG_DEBUG, "thread opaque outside:%p", h);
}
