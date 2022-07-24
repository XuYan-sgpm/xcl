//
// Created by xuyan on 2022/6/21.
//

#include "xcl/util/CBytes.h"
#include <string.h>

static bool __isCPUBigEndian()
{
    union
    {
        short val;
        char data[2];
    } unit;
    unit.val = 0x0001;
    return unit.data[1] == 1;
}

static void __translate(const void* src, void* dst, int size, bool big)
{
    if (big == __isCPUBigEndian())
    {
        memcpy(dst, src, size);
    }
    else
    {
        for (int i = 0; i < size; ++i)
        {
            ((char*)dst)[i] = ((const char*)src)[size - i - 1];
        }
    }
}

XCL_PUBLIC(void)
int32ToBytes(int val, void* dst, bool big)
{
    __translate(&val, dst, sizeof(int), big);
}

XCL_PUBLIC(int32_t)
bytesToInt32(const void* src, bool big)
{
    int result;
    __translate(src, &result, sizeof(int), big);
    return result;
}

XCL_PUBLIC(void)
int64ToBytes(int64_t val, void* dst, bool big)
{
    __translate(&val, dst, 8, big);
}

XCL_PUBLIC(int64_t)
bytesToInt64(const void* src, bool big)
{
    int64_t result;
    __translate(src, &result, 8, big);
    return result;
}

XCL_PUBLIC(void)
bytesCopy(const void* src, void* dst, int len, bool big)
{
    __translate(src, dst, len, big);
}
