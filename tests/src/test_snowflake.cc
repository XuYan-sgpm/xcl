#include <gtest/gtest.h>
#include <xcl/lang/system.h>
#include <xcl/util/algo.h>
#include <xcl/util/log.h>

TEST(SnowflakeTest, test1) {
  for (int i = 0; i < 100; i++) {
    LOG_P(LOG_DEBUG, "%llu", Snowflake_next());
    msleep(100);
  }
}
