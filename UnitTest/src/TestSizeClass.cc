//
// Created by 徐琰 on 2022/6/26.
//

#include <gtest/gtest.h>
#include "xcl/pool/SizeClass.h"
#include <iostream>
using namespace std;

TEST(SizeClassTest, func0) {
  SizeClass_initialize();
  cout << SizeClass_size() << endl;
  int32_t out[6];
  for (int i = 0; i < SizeClass_size(); i++) {
    memset(out, 0, sizeof(out));
    SizeClass_get(i, out);
    for (int j = 0; j < 6; j++) {
      cout << out[j] << ' ';
    }
    cout << endl;
  }
  SizeClass_finalize();
}

TEST(SizeClassTest, func1) {
  SizeClass_initialize();
  auto func = [](uint32_t size) -> uint32_t {
    int32_t out[6];
    for (int i = 0; i < SizeClass_size(); i++) {
      SizeClass_get(i, out);
      if (size == out[5]) {
        return size;
      }
      if (size < out[5]) {
        return out[5];
      }
    }
    return size;
  };
  for (uint32_t size = 1; size <= (1 << 24); size++) {
    auto ret = SizeClass_normalize(size);
    if (ret != func(size)) {
      cout << ret << "," << size << endl;
      ASSERT_TRUE(false);
    }
  }
  SizeClass_finalize();
}