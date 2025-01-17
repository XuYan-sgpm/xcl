#include <gtest/gtest.h>
#include <internal/util/log_manager.h>
#include <xcl/util/log.h>

#include <atomic>

class LogManagerTest : public testing::Test {
 protected:
  LogManager manager;

  void SetUp() {
    LogManageConfig config = {0};
    config.log_validity = 86500 * 5;
    config.max_logs = 2;
    config.single_log_limit = 1380 << 10;
    config.total_log_limit = 9880 << 10;
    ASSERT_TRUE(LogManager_init(&manager, config));
  }
  void TearDown() { LogManager_destroy(&manager); }
};

TEST_F(LogManagerTest, test1) {
  ASSERT_TRUE(LogManager_exec(&manager, Path_dirname(Path_module(nullptr)), false));
}
