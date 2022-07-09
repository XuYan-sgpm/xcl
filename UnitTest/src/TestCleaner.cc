//
// Created by xuyan on 2022/7/8.
//

#include <gtest/gtest.h>
#include <iostream>
#include "xcl/util/cleaner.h"

using namespace std;

static void
__clean(void*) {
  int a = 1;
}

TEST(CleanerTest, func1) {
  ASSERT_TRUE(__internal_registerCleanProc(__clean, NULL));
}