//
// Created by 徐琰 on 2024/2/22.
//

#include <gtest/gtest.h>
#include <sys/types.h>
#include <xcl/io/path.h>
#include <xcl/lang/system.h>
#include <xcl/util/log.h>

#ifdef Windows
#define _MODE 0
#else
#define _MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#endif

class PathTest : public testing::Test {
 protected:
  PathIter* iter = NULL;
  void SetUp() override {
    iter = Path_iter(Path_dirname(Path_module(NULL)));
    ASSERT_NE(iter, nullptr);
  }

  void TearDown() override { PathIter_close(iter); }

  void genFileTree(int level, int max_level, int& idx, std::string& path) {
    char name[256];
    if (level < max_level)
      sprintf(name, "dir%d", idx++);
    else
      sprintf(name, "file%d", idx++);
    unsigned old_size = path.length();
    path.push_back(Path_separator());
    path.append(name);
    ASSERT_TRUE(Path_create(path.c_str(), level < max_level));
    LOG_P(LOG_DEBUG, "create path %s", path.c_str());
    if (level < max_level) {
      for (int i = 0; i < max_level; i++)
        genFileTree(level + 1, max_level, idx, path);
    }
    path.erase(old_size, path.length() - old_size);
  }
};

TEST_F(PathTest, test1) {
  LOG_P(LOG_INFO, "path iter cwd:%s", PathIter_dir(iter));
  const char* name;
  while ((name = PathIter_next(iter))) {
    LOG_P(LOG_INFO, "list filename %s", name);
  }
}

TEST_F(PathTest, test2) {
  char out[1024];
  ASSERT_TRUE(Path_join(out, sizeof(out), "//xuyan", "\\c:windows\\", "//null\\1w2", nullptr));
  char out2[1024];
#ifdef Windows
  sprintf(out2, "%s%c%s%c%s", "xuyan", Path_separator(), "c:windows", Path_separator(),
          "null\\1w2");
#else
  sprintf(out2, "/%s%c%s%c%s", "xuyan", Path_separator(), "\\c:windows\\", Path_separator(),
          "null\\1w2");
#endif
  ASSERT_EQ(strcmp(out, out2), 0);
  String ret = Path_joins("//xuyan", "\\c:windows\\", "//null\\1w2", nullptr);
  ASSERT_EQ(strcmp(String_str(&ret), out2), 0);
  LOG_P(LOG_INFO, "join output:%s", out);
  String_destroy(&ret);
}

TEST_F(PathTest, test3) {
  char path[1024];
  ASSERT_TRUE(Path_join(path, sizeof(path), Path_dirname(Path_module(NULL)), "..", "..", NULL));
  std::string s = path;
  int idx = 1;
  genFileTree(0, 4, idx, s);
  LOG_P(LOG_INFO, "remove dir:%s", path);
  std::string entry = path;
  entry.push_back(Path_separator());
  entry.append("dir1");
  ASSERT_TRUE(Path_walk(entry.c_str(), Path_debugHandler()));
  std::string entry2 = path;
  entry2.push_back(Path_separator());
  entry2.append("xdir1");
  ASSERT_TRUE(Path_mkdir(false, entry2.c_str(), 0));
  ASSERT_TRUE(Path_copy(entry.c_str(), entry2.c_str(), PATH_COPY_OVERWRITE | PATH_COPY_ATTR));
  ASSERT_TRUE(Path_remove(entry.c_str()));
  ASSERT_TRUE(Path_remove(entry2.c_str()));
  ASSERT_FALSE(Path_exists(entry.c_str()));
  ASSERT_FALSE(Path_exists(entry2.c_str()));
}
