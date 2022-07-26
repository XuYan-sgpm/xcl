//
// Created by xuyan on 2022/7/25.
//
#include <gtest/gtest.h>
#include <iostream>
#include <xcl/util/CRingBuffer.h>
using namespace std;

TEST(TestRingBuffer, func1)
{
    CRingBuffer rb = RingBuffer_make(100, 4);
    ASSERT_NE(rb.buf, nullptr);
    ASSERT_EQ(RingBuffer_capacity(&rb), 100);
    ASSERT_EQ(RingBuffer_size(&rb), 0);
    ASSERT_EQ(RingBuffer_empty(&rb), true);
    for (int i = 0; i < 100; i++)
    {
        ASSERT_TRUE(RingBuffer_pushBack(&rb, &i, false));
    }
    for (int i = 0; i < 100; i++)
    {
        ASSERT_FALSE(RingBuffer_pushBack(&rb, &i, false));
        int val;
        ASSERT_TRUE(RingBuffer_get(&rb, i, &val));
        ASSERT_EQ(val, i);
    }
    for (int i = 0; i < 100; i++)
    {
        int val = 200 - i;
        ASSERT_TRUE(RingBuffer_pushFront(&rb, &val, true));
    }
    for (int i = 0; i < 100; i++)
    {
        int val;
        ASSERT_TRUE(RingBuffer_popFront(&rb, &val));
        ASSERT_EQ(val, 101 + i);
    }
    ASSERT_TRUE(RingBuffer_empty(&rb));
}