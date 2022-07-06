//
// Created by xuyan on 2022/6/30.
//

#include <gtest/gtest.h>
#include <util/CBits.h>

TEST(CBitsTest, func0) {
  //  auto bits = bits64();
  auto* bits = Bits_new(10240);
  for (int i = 0; i < Bits_len((CBits*)(bits)); i++) {
    ASSERT_FALSE(Bits_get((CBits*)bits, i));
    Bits_set((CBits*)bits, i, true);
    ASSERT_TRUE(Bits_get((CBits*)bits, i));
  }
  for (int i = 0; i < Bits_len((CBits*)(bits)); i++) {
    ASSERT_TRUE(Bits_get((CBits*)bits, i));
    Bits_set((CBits*)bits, i, false);
    ASSERT_FALSE(Bits_get((CBits*)bits, i));
  }
  Bits_delete(bits);
}