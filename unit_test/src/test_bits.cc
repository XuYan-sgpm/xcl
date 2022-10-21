//
// Created by xuyan on 2022/6/30.
//

#include "xcl/util/bits.h"
#include <gtest/gtest.h>

TEST(CBitsTest, func0)
{
    //  auto bits = bits64();
    auto* bits = Bits_new(10240);
    for (int i = 0; i < Bits_len((Bits*)(bits)); i++)
    {
        ASSERT_FALSE(Bits_get((Bits*)bits, i));
        Bits_set((Bits*)bits, i, true);
        ASSERT_TRUE(Bits_get((Bits*)bits, i));
    }
    for (int i = 0; i < Bits_len((Bits*)(bits)); i++)
    {
        ASSERT_TRUE(Bits_get((Bits*)bits, i));
        Bits_set((Bits*)bits, i, false);
        ASSERT_FALSE(Bits_get((Bits*)bits, i));
    }
    Bits_delete(bits);
}