//
// Created by xuyan on 2022/7/25.
//
#include <gtest/gtest.h>
#include <iostream>
#include <xcl/util/RingBuffer.h>
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

TEST(TestRingBuffer, func2)
{
    CRingBuffer rb = RingBuffer_make(245, 4);
    ASSERT_NE(rb.buf, nullptr);
    int val;
    for (int i = 0; i < RingBuffer_capacity(&rb); i++)
    {
        val = rand();
        ASSERT_TRUE(RingBuffer_insert(&rb, 0, &val, false));
        ASSERT_EQ(RingBuffer_size(&rb), i + 1);
    }
    for (int i = 0; i < RingBuffer_size(&rb); i++)
    {
        ASSERT_TRUE(RingBuffer_get(&rb, i, &val));
        cout << val << ' ';
    }
    cout << endl;
    val = 97;
    ASSERT_TRUE(RingBuffer_replaceRepeat(&rb, 56, 12, 3, &val));
    ASSERT_EQ(RingBuffer_size(&rb), 236);
    for (int i = 0; i < RingBuffer_size(&rb); i++)
    {
        ASSERT_TRUE(RingBuffer_get(&rb, i, &val));
        cout << val << ' ';
    }
    cout << endl;
}

TEST(TestRingBuffer, func3)
{
    CRingBuffer rb = RingBuffer_make(100, 8);
    ASSERT_NE(rb.buf, nullptr);
    double values[100] = {0};
    for (int i = 0; i < 100; i++)
    {
        values[i] = 3.12 * 7.32 * (rand());
    }
    ASSERT_TRUE(RingBuffer_insertRegion(&rb, 0, values, 8, 100, false));
    for (int i = 0; i < 100; i++)
    {
        double d;
        ASSERT_TRUE(RingBuffer_get(&rb, i, &d));
        ASSERT_EQ(d, values[i]);
    }
    ASSERT_EQ(RingBuffer_size(&rb), 100);
    RingBuffer_clear(&rb);
    ASSERT_EQ(rb.size, 0);
    ASSERT_FALSE(RingBuffer_get(&rb, 0, nullptr));
}