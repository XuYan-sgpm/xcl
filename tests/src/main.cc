//
// Created by 徐琰 on 2022/7/24.
//

#include <gtest/gtest.h>
#include <internal/util/str_impl.h>
#include <string.h>
#include <xcl/lang/system.h>

#include <atomic>

#ifdef VLD_ENABLE
#include <vld.h>
#endif

int main() {
  testing::GTEST_FLAG(repeat) = 1;
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}