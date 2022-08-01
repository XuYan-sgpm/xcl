//
// Created by xuyan on 2022/7/30.
//

#include <xcl/lang/CSys.h>

XCL_EXPORT bool XCL_API
isCpuBigEndian()
{
    union
    {
        short val;
        char data[2];
    } unit;
    unit.val = 0x0001;
    return unit.data[1] == 1;
}