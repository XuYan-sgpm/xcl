//
// Created by 14049 on 2024/5/8.
//

#include <gtest/gtest.h>
#include <xcl/concurrent/util/linked_blocking_queue.h>
#include <xcl/concurrent/util/thread_pool_executor.h>
#include <xcl/lang/thread.h>
#include <xcl/util/log.h>

#include <future>
#include <thread>

static std::atomic<int> id_gen(0);

struct Task1 : public Task {
  int ret = 0;

  Task1() {
    static void* task1_vptr[] = {(void*)Task1::exec};
    __vptr = (decltype(__vptr))task1_vptr;
  }

  static bool exec(Task* task) {
    ((Task1*)task)->ret = 78;
    int i = id_gen.fetch_add(1);
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(100));
    fprintf(stderr, "%dth sleep 100ms at thread %lu\n", i, Thread_currentId());
    return true;
  }
};

class ExecutorTest : public testing::Test {
 protected:
  ThreadPoolExecutor* executor;

  void SetUp() {
    id_gen = 0;
    executor = ThreadPoolExecutor_new(4, 20, 1000, NULL);
    ASSERT_NE(executor, nullptr);
  }

  void TearDown() { ThreadPoolExecutor_free(executor); }
};

TEST_F(ExecutorTest, testSimpleSubmit) {
  Task1 task1;
  auto sig = ThreadPoolExecutor_submit(executor, &task1);
  ASSERT_NE(sig, nullptr);
  ASSERT_EQ(TaskSignal_wait(sig, -1), TASK_STATE_COMPLETE);
  std::cout << task1.ret << std::endl;
  TaskSignal_free(sig);
}

TEST_F(ExecutorTest, test1000submit) {
  unsigned n = 1000;
  TaskSignal** signals = new TaskSignal*[n];
  Task1* tasks = new Task1[n];
  for (int i = 0; i < n; i++) {
    while (!(signals[i] = ThreadPoolExecutor_submit(executor, &tasks[i]))) {
    }
  }
  for (int i = 0; i < n; i++) {
    ASSERT_EQ(TaskSignal_wait(signals[i], -1), TASK_STATE_COMPLETE);
    ASSERT_EQ(tasks[i].ret, 78);
    TaskSignal_free(signals[i]);
  }
  ASSERT_EQ(id_gen.load(), n);
  delete[] signals;
  delete[] tasks;
}

TEST_F(ExecutorTest, test1000execute) {
  unsigned n = 1000;
  Task1* tasks = new Task1[n];
  for (int i = 0; i < n; i++) {
    while (!ThreadPoolExecutor_execute(executor, &tasks[i])) {
    }
  }
  ThreadPoolExecutor_free(executor);
  executor = nullptr;
  delete[] tasks;
}

TEST_F(ExecutorTest, test100execute) {
  unsigned n = 100;
  Task1* tasks = new Task1[n];
  for (int i = 0; i < n; i++) {
    while (!ThreadPoolExecutor_execute(executor, &tasks[i])) {
    }
  }
  ThreadPoolExecutor_free(executor);
  executor = nullptr;
  delete[] tasks;
}
