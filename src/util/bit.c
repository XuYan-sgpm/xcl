//
// Created by xuyan on 2022/6/29.
//

#include "util/bit.h"

uint8_t XCL_API
Bit8_set(uint8_t val, int32_t index, bool on)
{
    return on ? val | (1 << (index & 7)) : val & ~(1 << (index & 7));
}

bool XCL_API
Bit8_on(uint8_t val, int32_t index)
{
    return val & (1 << (index & 7));
}

uint16_t XCL_API
Bit16_set(uint16_t val, int32_t index, bool on)
{
    return on ? val | (1 << (index & 15)) : val & ~(1 << (index & 15));
}

bool XCL_API
Bit16_on(uint16_t val, int32_t index)
{
    return val & (1 << (index & 15));
}

uint32_t XCL_API
Bit32_set(uint32_t val, int32_t index, bool on)
{
    return on ? val | (1 << (index & 31)) : val & ~(1 << (index & 31));
}

bool XCL_API
Bit32_on(uint32_t val, int32_t index)
{
    return val & (1 << (index & 31));
}

uint64_t XCL_API
Bit64_set(uint64_t val, int32_t index, bool on)
{
    return on ? val | (1 << (index & 63)) : val & ~(1 << (index & 63));
}

bool XCL_API
Bit64_on(uint64_t val, int32_t index)
{
    return val & (1 << (index & 63));
}

void XCL_API
Bit_set(void* obj, int32_t index, bool on)
{
    char* p = (char*)obj + (index >> 3);
    *p = on ? *p | (1 << (index & 7)) : *p & ~(1 << (index & 7));
}

bool XCL_API
Bit_on(const void* obj, int32_t index)
{
    return *((char*)obj + (index >> 3)) & (1 << (index & 7));
}
