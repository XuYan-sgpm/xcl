//
// Created by xuyan on 2022/6/30.
//

#include <gtest/gtest.h>
#include <util/CBits.h>

static void __assertMsg(bool cond, const char *msg) {
  if (!cond) {
    fprintf(stderr, "%s\n", msg);
  }
  assert(cond);
}

TEST(CBitsTest, func0) {
  //  auto bits = bits64();
  __assertMsg(false, "assert false with msg");
  auto *bits = Bits_new(456);
  std::cout << Bits_get(bits, 32) << std::endl;
  for (int i = 0; i < Bits_len((CBits *)(&bits)); i++) {
    std::cout << i << std::endl;
    if (i == 32) {
      int a = 1;
    }
    ASSERT_FALSE(Bits_get((CBits *)&bits, i));
    Bits_set((CBits *)&bits, i, true);
    ASSERT_TRUE(Bits_get((CBits *)&bits, i));
  }
  Bits_delete(bits);
}