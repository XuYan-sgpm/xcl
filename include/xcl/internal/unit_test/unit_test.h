//
// Created by 徐琰 on 2022/2/8.
//

#ifndef SCL_TEST_UNIT_TEST_H_
#define SCL_TEST_UNIT_TEST_H_

class UnitTest {
  struct TestNode {
    void (*test_func)() = nullptr;
    TestNode *next = nullptr;
  };
  TestNode head_;
  TestNode *tail_ = &head_;

public:
  void Add(void (*func)());
  void Test() const;
  void TestFiles(const char *test_dir, unsigned n, ...);

public:
  ~UnitTest();

private:
  UnitTest() = default;

private:
  static UnitTest *unit_test_;

public:
  static UnitTest &Instance();
};

#endif // SCL_TEST_UNIT_TEST_H_
