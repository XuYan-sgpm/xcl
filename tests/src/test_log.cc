//
// Created by 徐琰 on 2024/2/2.
//

#include <gtest/gtest.h>
#include <xcl/lang/system.h>
#include <xcl/util/log.h>

#include <thread>

#define THREADS 8

class LogTest : public testing::Test {
 protected:
  void* log = nullptr;

  void config() {
    String log_file_fmt = Path_joins(Path_dirname(Path_module(nullptr)),
                                     "xcl_log_${year}-${month}-${day}.log", nullptr);
    LogConfig log_config = {"zdxlz", "", String_str(&log_file_fmt), LOG_VERBOSE, LOG_INFO};
    LogManageConfig log_manage_cfg = {-1, -1, -1, -1, nullptr, nullptr};
    Log_config(log_config, log_manage_cfg);
    String_destroy(&log_file_fmt);
  }

  void SetUp() { config(); }

  template <typename Callable>
  void run(const Callable& callable) {
    std::thread threads[THREADS];
    for (int i = 0; i < THREADS; i++)
      threads[i] = std::thread(callable);
    for (int i = 0; i < THREADS; i++)
      threads[i].join();
  }
};

TEST_F(LogTest, func1) {
  run([&] {
    const char* name = "xuyan";
    for (int i = 0; i < 100; i++)
      LOG_I("hello world");
    for (int i = 0; i < 100; i++)
      LOG_P(LOG_WARNING, "warning message");
  });
}

TEST_F(LogTest, test2) {
  run([&] {
    for (int i = 0; i < 10; i++)
      LOG_D("this may not write");
    for (int i = 0; i < 10; i++)
      LOG_I("this must be writen");
  });
}

TEST_F(LogTest, test3) {
  config();
  run([&] {
    for (int i = 0; i < 10; i++)
      LOG_D("this may not write");
    for (int i = 0; i < 10; i++)
      LOG_I("this must be writen");
  });
}

TEST_F(LogTest, test4) {
  for (int i = 0; i < 10; i++)
    config();
}
