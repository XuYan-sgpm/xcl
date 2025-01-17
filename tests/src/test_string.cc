//
// Created by 徐琰 on 2023/12/11.
//

#include <gtest/gtest.h>
#include <xcl/util/string.h>

#include <iostream>
#include <string>

class StringTest : public ::testing::Test {
 protected:
  String s;

  void SetUp() override { s = String_make(); }

  void TearDown() override { String_destroy(&s); }

  int _random(int l, int r) {
    if (r - l < 0)
      abort();
    if (r - l == 0)
      return l;
    srand(time(nullptr) + rand());
    unsigned region = r - l + 1;
    unsigned val = rand();
    return l + (val % region);
  }
};

TEST_F(StringTest, test1) {
  ASSERT_TRUE(String_append(&s, "hello world", -1));
  ASSERT_EQ(strcmp((char*)String_str(&s), "hello world"), 0);
  ASSERT_TRUE(String_append(&s, "my name is xuyan", -1));
  ASSERT_EQ(strcmp((char*)String_str(&s), "hello worldmy name is xuyan"), 0);
  ASSERT_TRUE(String_fill(&s, "assign", -1));
  ASSERT_EQ(strcmp((char*)String_str(&s), "assign"), 0);
  ASSERT_TRUE(String_filln(&s, 34, 'y'));
  ASSERT_EQ(String_size(&s), 34);
  for (int i = 0; i < 34; i++)
    ASSERT_EQ(*(char*)String_at(&s, i), 'y');
}

TEST_F(StringTest, func3) {
  std::string s2;
  const char* p = "insert data";
  for (int i = 0; i < 128; i++) {
    int pos = _random(0, s2.size());
    s2.insert(s2.begin() + pos, p, p + strlen(p));
    ASSERT_TRUE(String_insert(&s, pos, p, -1));
    ASSERT_EQ(strcmp(String_str(&s), s2.c_str()), 0);
    int n;
    while ((n = _random(0, s2.size())) <= 0) {
    }
    int pos2 = _random(0, s2.size() - n);
    s2.erase(s2.begin() + pos2, s2.begin() + pos2 + n);
    String_erase(&s, pos2, n);
    ASSERT_EQ(strcmp(String_str(&s), s2.c_str()), 0);
  }
}

TEST_F(StringTest, func4) {
  int n = _random(1, 4593);
  for (int i = 0; i < n; i++)
    ASSERT_TRUE(String_push(&s, 'x'));
  ASSERT_EQ(String_size(&s), n);
  std::string s2;
  s2.assign(n, 'x');
  ASSERT_EQ(strncmp(String_str(&s), s2.c_str(), s2.length()), 0);
  for (int i = 0; i < n; i++)
    ASSERT_EQ(String_pop(&s), 'x');
  ASSERT_EQ(String_size(&s), 0);
}

TEST_F(StringTest, func5) {
  ASSERT_TRUE(String_fill(&s, "hello world", -1));
  String s2 = String_from("hello world", -1);
  ASSERT_EQ(String_compare(&s, &s2), 0);
  int pos = String_finds(&s, 0, "world");
  ASSERT_EQ(pos, 6);
}
