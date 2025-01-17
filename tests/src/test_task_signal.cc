#include <gtest/gtest.h>
#include <xcl/concurrent/util/task_signal.h>

#include <chrono>
#include <thread>

extern "C" {
TaskSignal* TaskSignal_new();
TaskSignal* TaskSignal_ref(TaskSignal* sig);
}

class TaskSignalTest : public testing::Test {
 protected:
  TaskSignal* sig;

  void SetUp() {
    sig = TaskSignal_new();
    ASSERT_NE(sig, nullptr);
  }
  void TearDown() { TaskSignal_free(sig); }
};

TEST_F(TaskSignalTest, test1) {
  std::thread t([&] { TaskSignal_wait(sig, -1); });
  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000));
  ASSERT_TRUE(TaskSignal_trigger(sig, TASK_STATE_CANCELLED));
  t.join();
}
