//
// Created by xuyan on 2022/6/30.
//

#include "xcl/util/bit.h"
#include <gtest/gtest.h>
#include <iostream>
#include <xcl/lang/system.h>
using namespace std;

TEST(BIT, func)
{
    uint8_t val1 = 0;
    for (int i = 0; i < sizeof(val1) << 3; ++i)
    {
        val1 = Bit8_set(val1, i, true);
        ASSERT_TRUE(Bit8_on(val1, i));
        val1 = Bit8_set(val1, i, false);
        ASSERT_FALSE(Bit8_on(val1, i));
    }

    uint16_t val2 = 0;
    for (int i = 0; i < sizeof(val2) << 3; ++i)
    {
        val2 = Bit16_set(val2, i, true);
        ASSERT_TRUE(Bit16_on(val2, i));
        val2 = Bit16_set(val2, i, false);
        ASSERT_FALSE(Bit16_on(val2, i));
    }

    uint32_t val3 = 0;
    for (int i = 0; i < sizeof(val3) << 3; ++i)
    {
        val3 = Bit32_set(val3, i, true);
        ASSERT_TRUE(Bit32_on(val3, i));
        val3 = Bit32_set(val3, i, false);
        ASSERT_FALSE(Bit32_on(val3, i));
    }

    uint64_t val4 = 0;
    for (int i = 0; i < sizeof(val4) << 3; ++i)
    {
        val4 = Bit64_set(val4, i, true);
        ASSERT_TRUE(Bit64_on(val4, i));
        val4 = Bit64_set(val4, i, false);
        ASSERT_FALSE(Bit64_on(val4, i));
    }

    unsigned char buf[512] = {0};
    for (int i = 0; i < sizeof(buf) << 3; ++i)
    {
        Bit_set(buf, i, true);
        ASSERT_TRUE(Bit_on(buf, i));
    }
    for (int i = 0; i < sizeof(buf) << 3; ++i)
    {
        Bit_set(buf, i, false);
        ASSERT_FALSE(Bit_on(buf, i));
    }
    cout << "bit test pass" << endl;
}