//
// Created by 徐琰 on 2022/6/26.
//

#include <gtest/gtest.h>
#include <pool/SizeClass.h>

TEST(SizeClassTest, func0) { ASSERT_EQ(SizeClass_initialize(), true); }