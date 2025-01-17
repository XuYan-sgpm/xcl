//
// Created by 14049 on 2023/12/20.
//

#include <gtest/gtest.h>
#include <limits.h>
#include <xcl/util/byte_buffer.h>

#include <iostream>

class ByteBufferTest : public ::testing::Test {
 protected:
  ByteBuffer bb_;

  void SetUp() override {
    Test::SetUp();
    auto endian = (ByteEndian)get_random(0, 1);
    bb_ = ByteBuffer_alloc(1024, endian);
    ASSERT_NE(bb_.ptr, nullptr);
    std::cout << (endian == BIG ? "big" : "little") << '\n';
    if (endian != localEndian())
      std::cout << "local endian not equal to specified" << '\n';
  }

  void TearDown() override {
    Test::TearDown();
    ByteBuffer_destroy(&bb_);
  }

  int get_random(int l, int r) {
    if (l == r)
      return l;
    srand(time(nullptr) + rand());
    unsigned val = rand();
    val %= (r - l + 1);
    if (val < l || val > r)
      abort();
    return l + val;
  }

  template <typename T>
  void test_put(T val) {
    unsigned old = bb_.cursor;
    switch (sizeof(val)) {
    case 1: {
      ASSERT_TRUE(ByteBuffer_put8(&bb_, val));
      ASSERT_EQ(ByteBuffer_get8(&bb_, old), val);
      break;
    }
    case 2: {
      ASSERT_TRUE(ByteBuffer_put16(&bb_, val));
      ASSERT_EQ(ByteBuffer_get16(&bb_, old), val);
      break;
    }
    case 4: {
      ASSERT_TRUE(ByteBuffer_put32(&bb_, val));
      ASSERT_EQ(ByteBuffer_get32(&bb_, old), val);
      break;
    }
    case 8: {
      ASSERT_TRUE(ByteBuffer_put64(&bb_, val));
      ASSERT_EQ(ByteBuffer_get64(&bb_, old), val);
      break;
    }
    }
    ASSERT_EQ(bb_.cursor, old + sizeof(val));
  }

  template <typename T>
  void test_set(unsigned offset, T val) {
    switch (sizeof(val)) {
    case 1: {
      ASSERT_TRUE(ByteBuffer_set8(&bb_, offset, val));
      ASSERT_EQ(ByteBuffer_get8(&bb_, offset), val);
      break;
    }
    case 2: {
      ASSERT_TRUE(ByteBuffer_set16(&bb_, offset, val));
      ASSERT_EQ(ByteBuffer_get16(&bb_, offset), val);
      break;
    }
    case 4: {
      ASSERT_TRUE(ByteBuffer_set32(&bb_, offset, val));
      ASSERT_EQ(ByteBuffer_get32(&bb_, offset), val);
      break;
    }
    case 8: {
      ASSERT_TRUE(ByteBuffer_set64(&bb_, offset, val));
      ASSERT_EQ(ByteBuffer_get64(&bb_, offset), val);
      break;
    }
    }
  }

  template <typename T>
  T test_take() {
    switch (sizeof(T)) {
    case 1: {
      return ByteBuffer_take8(&bb_);
    }
    case 2: {
      return ByteBuffer_take16(&bb_);
    }
    case 4: {
      return ByteBuffer_take32(&bb_);
    }
    case 8: {
      return ByteBuffer_take64(&bb_);
    }
    default:
      abort();
    }
  }

  template <typename T>
  T test_get(unsigned offset) {
    switch (sizeof(T)) {
    case 1: {
      return ByteBuffer_get8(&bb_, offset);
    }
    case 2: {
      return ByteBuffer_get16(&bb_, offset);
    }
    case 4: {
      return ByteBuffer_get32(&bb_, offset);
    }
    case 8: {
      return ByteBuffer_get64(&bb_, offset);
    }
    default:
      abort();
    }
  }
};

TEST_F(ByteBufferTest, func1) {
  for (int i = 0; i < 1024; i++) {
    unsigned char val = get_random(0, 255);
    test_put(val);
    test_set(get_random(0, 1023), val);
  }
  ASSERT_EQ(bb_.cursor, bb_.limit);
  ASSERT_EQ(bb_.cursor, bb_.cap);
  ByteBuffer_flip(&bb_);
  for (int i = 0; i < 1024; i++)
    test_take<unsigned char>();
  ASSERT_EQ(bb_.cursor, bb_.limit);
  ByteBuffer_rewind(&bb_);
  for (int i = 0; i < 512; i++) {
    unsigned short val = get_random(0, 65535);
    test_put(val);
    test_set(get_random(0, 1022), val);
  }
  ByteBuffer_flip(&bb_);
  for (int i = 0; i < 512; i++)
    test_take<unsigned short>();
  ASSERT_EQ(bb_.cursor, bb_.limit);
  ByteBuffer_rewind(&bb_);
  for (int i = 0; i < 256; i++) {
    unsigned val = get_random(0, INT_MAX);
    test_put(val);
    test_set(get_random(0, 1020), val);
  }
  ByteBuffer_flip(&bb_);
  for (int i = 0; i < 256; i++)
    test_take<unsigned>();
  ASSERT_EQ(bb_.cursor, bb_.limit);
  ByteBuffer_rewind(&bb_);
}
