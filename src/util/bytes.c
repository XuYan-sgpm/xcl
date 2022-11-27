//
// Created by xuyan on 2022/6/21.
//

#include "util/bytes.h"
#include "lang/system.h"
#include <string.h>

static void
__translate(const void* src, void* dst, int size, bool big)
{
    if (big == bigEndian())
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

void XCL_API
int32ToBytes(int val, void* dst, bool big)
{
    __translate(&val, dst, sizeof(int), big);
}

int32_t XCL_API
bytesToInt32(const void* src, bool big)
{
    int result;
    __translate(src, &result, sizeof(int), big);
    return result;
}

void XCL_API
int64ToBytes(int64_t val, void* dst, bool big)
{
    __translate(&val, dst, 8, big);
}

int64_t XCL_API
bytesToInt64(const void* src, bool big)
{
    int64_t result;
    __translate(src, &result, 8, big);
    return result;
}

void XCL_API
bytesCopy(const void* src, void* dst, int len, bool big)
{
    __translate(src, dst, len, big);
}
