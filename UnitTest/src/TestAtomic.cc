//
// Created by xuyan on 2022/7/29.
//
#include <gtest/gtest.h>
#include <iostream>
using namespace std;

extern "C" void
__testAtomic();

TEST(TestAtomic, func)
{
    __testAtomic();
}
