//
// Created by xuyan on 2022/6/29.
//

#include "util/bits.h"
#include "pool/pool.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static inline void
__Bits_setLen(Bits* bits, const int32_t category)
{
    memcpy((void*)&bits->n_bits, &category, sizeof(category));
}

static inline char*
__Bits_getValPtr(Bits* bits)
{
    return ((BitsSet*)bits)->ctx;
}

XCL_EXPORT Bits* XCL_API
Bits_new(int32_t size)
{
    if ((size & 7))
    {
        size = ((size >> 3) + 1) << 3;
    }
    Bits* bits = Pool_alloc(Pool_def(), (size >> 3) + sizeof(Bits));
    if (!bits)
    {
        return NULL;
    }
    __Bits_setLen(bits, size >> 3);
    memset(__Bits_getValPtr(bits), 0, size >> 3);
    return bits;
}

XCL_EXPORT void XCL_API
Bits_delete(Bits* bits)
{
    Pool_dealloc(Pool_def(), bits, sizeof(Bits) + (bits->n_bits >> 3));
}

XCL_EXPORT void XCL_API
Bits_set(Bits* bits, int32_t idx, bool on)
{
    assert((idx >= 0 && idx < bits->n_bits) && "index overflow");
    unsigned char* vp = (unsigned char*)__Bits_getValPtr(bits) + (idx >> 3);
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

XCL_EXPORT int32_t XCL_API
Bits_len(Bits* bits)
{
    return bits->n_bits;
}

XCL_EXPORT bool XCL_API
Bits_get(Bits* bits, int32_t idx)
{
    unsigned char* vp = (unsigned char*)__Bits_getValPtr(bits) + (idx >> 3);
    unsigned char value = vp[0];
    unsigned char op = 1 << (idx & 7);
    return value & op;
}

XCL_EXPORT Bits64 XCL_API
bits64()
{
    return (Bits64){{64}, 0};
}

XCL_EXPORT Bits8 XCL_API
bits8()
{
    return (Bits8){{8}, 0};
}

XCL_EXPORT Bits16 XCL_API
bits16()
{
    return (Bits16){{16}, 0};
}

XCL_EXPORT Bits32 XCL_API
bits32()
{
    return (Bits32){{32}, 0};
}

XCL_EXPORT unsigned char XCL_API
Bits_getByte(Bits* bits, int32_t idx)
{
    return *(unsigned char*)(__Bits_getValPtr(bits) + idx);
}
