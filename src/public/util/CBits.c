//
// Created by xuyan on 2022/6/29.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "xcl/util/CBits.h"

static inline void
__Bits_setLen(CBits*bits, const int32_t category)
{
    memcpy((void*)&bits->nBits, &category, sizeof(category));
}

static inline char*
__Bits_getValPtr(CBits*bits)
{
    return ((CBitsSet*)bits)->ctx;
}

XCL_PUBLIC(CBits*)
Bits_new(int32_t size)
{
    if ((size & 7))
    {
        size = ((size >> 3) + 1) << 3;
    }
    CBits*bits = malloc((size >> 3) + sizeof(CBits));
    if (!bits)
    {
        return NULL;
    }
    __Bits_setLen(bits, size >> 3);
    memset(__Bits_getValPtr(bits), 0, size >> 3);
    return bits;
}

XCL_PUBLIC(void)
Bits_delete(CBits*bits)
{ free(bits); }

XCL_PUBLIC(void)
Bits_set(CBits*bits, int32_t idx, bool on)
{
    assert((idx >= 0 && idx < bits->nBits) && "index overflow");
    unsigned char*vp = (unsigned char*)__Bits_getValPtr(bits) + (idx >> 3);
    unsigned char value = vp[0];
    if (on)
    {
        unsigned char op = 1 << (idx & 7);
        vp[0] = value | op;
    }
    else
    {
        unsigned char op = ~(1 << (idx & 7));
        vp[0] = value & op;
    }
}

XCL_PUBLIC(int32_t)
Bits_len(CBits*bits)
{ return bits->nBits; }

XCL_PUBLIC(bool)
Bits_get(CBits*bits, int32_t idx)
{
    unsigned char*vp = (unsigned char*)__Bits_getValPtr(bits) + (idx >> 3);
    unsigned char value = vp[0];
    unsigned char op = 1 << (idx & 7);
    return value & op;
}

XCL_PUBLIC(CBits64)
bits64()
{ return (CBits64){{64}, 0}; }

XCL_PUBLIC(CBits8)
bits8()
{ return (CBits8){{8}, 0}; }

XCL_PUBLIC(CBits16)
bits16()
{ return (CBits16){{16}, 0}; }

XCL_PUBLIC(CBits32)
bits32()
{ return (CBits32){{32}, 0}; }

XCL_PUBLIC(unsigned char)
Bits_getByte(CBits*bits, int32_t idx)
{
    return *(unsigned char*)(__Bits_getValPtr(bits) + idx);
}
