//
// Created by xuyan on 2022/7/21.
//

#include "util/algo.h"
#include <string.h>
#include <stdlib.h>

uint32_t XCL_API
_log2(uint64_t val)
{
    if (val == 0)
    {
        return 32;
    }
    uint32_t n = 1;
    if ((val >> 16) == 0)
    {
        n += 16;
        val <<= 16;
    }
    if ((val >> 24) == 0)
    {
        n += 8;
        val <<= 8;
    }
    if ((val >> 28) == 0)
    {
        n += 4;
        val <<= 4;
    }
    if ((val >> 30) == 0)
    {
        n += 2;
        val <<= 2;
    }
    n -= (val >> 31u);
    return 31u - n;
}
