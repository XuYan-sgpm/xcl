#include <gtest/gtest.h>
#include <internal/util/log_file_writer.h>
#include <xcl/io/path.h>
#include <xcl/util/log.h>

class LogFileWriterTest : public testing::Test {
 protected:
  LogFileWriter writer;
  LogManager manager;
  LogManageConfig config = {0};

  void cleanLogs() {
    bool (*func)(PathHandler*, const char*) = [](PathHandler* obj, const char* path) -> bool {
      const char* name = Path_basename(path);
      if (strncmp(name, "test_log_", 9))
        return true;
      const char* ext = strrchr(name, '.');
      if (strcmp(ext, ".log"))
        return true;
      LOG_ASSERT(remove(path) == 0);
      LOG_P(LOG_DEBUG, "remove test log %s", path);
      return true;
    };
    void* vptr[] = {func};
    PathHandler cb;
    cb.__vptr = (decltype(cb.__vptr))vptr;
    Path_listDir(Path_dirname(Path_module(nullptr)), &cb);
  }

  void SetUp() {
    cleanLogs();
    config.filter = nullptr;
    config.log_validity = -1;
    config.max_logs = 7;
    config.single_log_limit = 128;
    config.total_log_limit = 1024;
    ASSERT_TRUE(LogManager_init(&manager, config));
    String log_file_fmt = Path_joins(Path_dirname(Path_module(nullptr)),
                                     "test_log_${year}_${month}_${day}.log", nullptr);
    ASSERT_TRUE(LogFileWriter_init(&writer, String_str(&log_file_fmt), &manager));
    String_destroy(&log_file_fmt);
  }
  void TearDown() {
    LogManager_destroy(&manager);
    LogFileWriter_destroy(&writer);
  }
};

TEST_F(LogFileWriterTest, test1) {
  LogCtx log_ctx;
  time_t now = time(nullptr);
  log_ctx.time_stamp = *localtime(&now);
  const char* msg = "hello world!my name is xuyan, i am programing\n";
  for (int i = 0; i < 100; i++) {
    ASSERT_TRUE(LogFileWriter_write(&writer, msg, strlen(msg), &log_ctx));
  }
}
