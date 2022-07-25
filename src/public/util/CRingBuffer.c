//
// Created by xuyan on 2022/7/13.
//

#include <string.h>
#include "xcl/util/CRingBuffer.h"
#include "xcl/pool/CPool.h"

XCL_PUBLIC(CRingBuffer)
RingBuffer_make(int32_t cap, int32_t bs)
{
    char* p = Pool_alloc(Pool_def(), ((int64_t)(cap)) * bs);
    if (p)
    {
        return (CRingBuffer){p, 0, 0, cap, bs};
    }
    else
    {
        return (CRingBuffer){p, 0, 0, 0, 0};
    }
}

XCL_PUBLIC(void)
RingBuffer_release(CRingBuffer* ringBuffer)
{
    if (ringBuffer->buf)
    {
        Pool_dealloc(Pool_def(),
                     ringBuffer->buf,
                     ((int64_t)(ringBuffer->cap)) * ringBuffer->blockSize);
    }
    memset(ringBuffer, 0, sizeof(CRingBuffer));
}

XCL_PUBLIC(int32_t)
RingBuffer_size(const CRingBuffer* ringBuffer)
{
    return (ringBuffer->end - ringBuffer->beg) / ringBuffer->blockSize;
}

XCL_PUBLIC(int32_t)
RingBuffer_capacity(const CRingBuffer* ringBuffer)
{
    return ringBuffer->cap;
}

XCL_PUBLIC(bool)
RingBuffer_empty(const CRingBuffer* ringBuffer)
{
    return ringBuffer->end - ringBuffer->beg == 0;
}

static bool __RingBuffer_checkPosition(const CRingBuffer* ringBuffer,
                                       int32_t pos)
{
    return pos >= 0 && pos < RingBuffer_size(ringBuffer);
}

static bool __RingBuffer_checkRegion(const CRingBuffer* ringBuffer,
                                     bool add,
                                     int32_t pos,
                                     int32_t len)
{
    int32_t size = RingBuffer_size(ringBuffer);
    if (add)
    {
        if (pos < 0 || len < 0 || pos > size)
        {
            return false;
        }
        if (len - (RingBuffer_capacity(ringBuffer) - size) > 0)
        {
            return false;
        }
    }
    else
    {
        if (pos < 0 || len < 0 || pos + len > size)
        {
            return false;
        }
    }
    return true;
}

static inline int32_t __RingBuffer_map(const CRingBuffer* ringBuffer,
                                       int32_t pos)
{
    return (ringBuffer->beg + pos) % ringBuffer->cap;
}

static inline int32_t
__RingBuffer_next(const CRingBuffer* ringBuffer, int32_t pos, int32_t step)
{
    return (pos + step) % (ringBuffer->cap);
}

static inline int64_t __RingBuffer_rightBytes(const CRingBuffer* ringBuffer,
                                              int32_t pos)
{
    int64_t ret = ringBuffer->cap - pos;
    ret *= ringBuffer->blockSize;
    return ret;
}

static void __RingBuffer_move(CRingBuffer* ringBuffer,
                              int32_t dstIdx,
                              int32_t srcIdx,
                              int32_t len)
{
    int64_t total = ((int64_t)len) * ringBuffer->blockSize;
    int64_t copied = 0;
    char* dst = RingBuffer_at(ringBuffer, dstIdx);
    char* src = RingBuffer_at(ringBuffer, srcIdx);
    if (dstIdx < srcIdx)
    {}
}
