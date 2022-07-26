//
// Created by xuyan on 2022/7/13.
//

#include <string.h>
#include "xcl/util/CRingBuffer.h"
#include "xcl/pool/CPool.h"

XCL_PUBLIC(CRingBuffer)
RingBuffer_make(int32_t cap, int32_t es)
{
    char* p = Pool_alloc(Pool_def(), ((int64_t)(cap)) * es);
    if (p)
    {
        return (CRingBuffer){p, 0, 0, cap, es};
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
                     ((int64_t)(ringBuffer->cap)) * ringBuffer->eleSize);
    }
    memset(ringBuffer, 0, sizeof(CRingBuffer));
}

XCL_PUBLIC(int32_t)
RingBuffer_size(const CRingBuffer* ringBuffer)
{
    return ringBuffer->size;
}

XCL_PUBLIC(int32_t)
RingBuffer_capacity(const CRingBuffer* ringBuffer)
{
    return ringBuffer->cap;
}

XCL_PUBLIC(bool)
RingBuffer_empty(const CRingBuffer* ringBuffer)
{
    return ringBuffer->size == 0;
}

static bool
__RingBuffer_checkPosition(const CRingBuffer* ringBuffer,
                           bool insert,
                           int32_t pos)
{
    return pos >= 0
           && (!insert ? pos < RingBuffer_size(ringBuffer)
                       : pos <= RingBuffer_size(ringBuffer));
}

static bool
__RingBuffer_checkRegion(const CRingBuffer* ringBuffer,
                         bool insert,
                         int32_t pos,
                         int32_t len)
{
    int32_t size = RingBuffer_size(ringBuffer);
    if (insert)
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

static inline int32_t
__RingBuffer_offset(const CRingBuffer* ringBuffer, int32_t off)
{
    off = off >= 0 ? off % ringBuffer->cap : -((-off) % ringBuffer->cap);
    return off;
}

static inline int32_t
__RingBuffer_map(const CRingBuffer* ringBuffer, int32_t pos)
{
    return (ringBuffer->beg + ringBuffer->cap
            + __RingBuffer_offset(ringBuffer, pos))
           % ringBuffer->cap;
}

static inline int32_t
__RingBuffer_mappedNext(const CRingBuffer* ringBuffer,
                        int32_t mappedPos,
                        int32_t step)
{

    return (mappedPos + __RingBuffer_offset(ringBuffer, step))
           % (ringBuffer->cap);
}

static inline void
__RingBuffer_nextBeg(CRingBuffer* ringBuffer, int32_t step)
{
    ringBuffer->beg = __RingBuffer_map(ringBuffer, step);
}

static void
__RingBuffer_move(CRingBuffer* ringBuffer,
                  int32_t dstIdx,
                  int32_t srcIdx,
                  int32_t len)
{
    if (dstIdx == srcIdx || len == 0)
    {
        return;
    }
    int32_t total = 0;
    int32_t es = ringBuffer->eleSize;
    char* buf = ringBuffer->buf;
    if (dstIdx < srcIdx)
    {
        int32_t to = __RingBuffer_map(ringBuffer, dstIdx);
        int32_t from = __RingBuffer_map(ringBuffer, srcIdx);
        while (total < len)
        {
            int32_t copied = ringBuffer->cap - (to > from ? to : from);
            if (copied > len - total)
                copied = len - total;
            memcpy(buf + to * es, buf + from * es, copied * es);
            to = __RingBuffer_mappedNext(ringBuffer, to, copied);
            from = __RingBuffer_mappedNext(ringBuffer, from, copied);
            total += copied;
        }
    }
    else
    {
        int32_t to = __RingBuffer_map(ringBuffer, dstIdx + len);
        int32_t from = __RingBuffer_map(ringBuffer, srcIdx + len);
        while (total < len)
        {
            if (to == 0)
                to = ringBuffer->cap;
            if (from == 0)
                from = ringBuffer->cap;
            int32_t copied = to > from ? from : to;
            if (copied > len - total)
                copied = len - total;
            memmove(buf + (to - copied) * es,
                    buf + (from - copied) * es,
                    copied * es);
            to = __RingBuffer_mappedNext(ringBuffer, to, -copied);
            from = __RingBuffer_mappedNext(ringBuffer, from, -copied);
            total += copied;
        }
    }
}

static void
__RingBuffer_copyExternalData(CRingBuffer* ringBuffer,
                              int32_t dstIdx,
                              const void* src,
                              int32_t len)
{
    if (len == 0)
    {
        return;
    }
    int32_t total = 0;
    int32_t es = ringBuffer->eleSize;
    char* buf = ringBuffer->buf;
    int32_t to = __RingBuffer_map(ringBuffer, dstIdx);
    while (total < len)
    {
        int32_t copied = ringBuffer->cap - to;
        if (copied > len - total)
            copied = len - total;
        memcpy(buf + to * es, src, copied * es);
        to = __RingBuffer_mappedNext(ringBuffer, to, copied);
        src = (const char*)src + copied * es;
        total += copied;
    }
}

static void
__RingBuffer_copyExternalBuf(CRingBuffer* ringBuffer,
                             int32_t dstIdx,
                             const CRingBuffer* src,
                             int32_t srcIdx,
                             int32_t len)
{
    if (len == 0)
    {
        return;
    }
    int32_t total = 0;
    int32_t es = ringBuffer->eleSize;
    int32_t to = __RingBuffer_map(ringBuffer, dstIdx);
    int32_t from = __RingBuffer_map(src, srcIdx);
    while (total < len)
    {
        int32_t copied = ringBuffer->cap - to;
        if (copied > src->cap - from)
            copied = src->cap - from;
        if (copied > len - total)
            copied = len - total;
        memcpy(ringBuffer->buf + to * es, src->buf + from * es, copied * es);
        to = __RingBuffer_mappedNext(ringBuffer, to, copied);
        from = __RingBuffer_mappedNext(src, from, copied);
        total += copied;
    }
}

XCL_PUBLIC(bool)
RingBuffer_pushFront(CRingBuffer* ringBuffer, const void* src, bool force)
{
    if (!__RingBuffer_checkRegion(ringBuffer, true, 0, 1) && !force)
    {
        return false;
    }
    int32_t to = __RingBuffer_map(ringBuffer, -1);
    memcpy(ringBuffer->buf + to * ringBuffer->eleSize,
           src,
           ringBuffer->eleSize);
    __RingBuffer_nextBeg(ringBuffer, -1);
    return true;
}

XCL_PUBLIC(bool)
RingBuffer_pushBack(CRingBuffer* ringBuffer, const void* src, bool force)
{
    if (!__RingBuffer_checkRegion(ringBuffer,
                                  true,
                                  RingBuffer_size(ringBuffer),
                                  1)
        && !force)
    {
        return false;
    }
    int32_t to = __RingBuffer_map(ringBuffer, RingBuffer_size(ringBuffer));
    memcpy(ringBuffer->buf + to * ringBuffer->eleSize,
           src,
           ringBuffer->eleSize);
    ++ringBuffer->size;
    return true;
}

XCL_PUBLIC(bool)
RingBuffer_popFront(CRingBuffer* ringBuffer, void* dst)
{
    if (RingBuffer_empty(ringBuffer))
    {
        return false;
    }
    int32_t from = __RingBuffer_map(ringBuffer, 0);
    memcpy(dst,
           ringBuffer->buf + from * ringBuffer->eleSize,
           ringBuffer->eleSize);
    __RingBuffer_nextBeg(ringBuffer, 1);
    --ringBuffer->size;
    return true;
}

XCL_PUBLIC(bool)
RingBuffer_popBack(CRingBuffer* ringBuffer, void* dst)
{
    if (RingBuffer_empty(ringBuffer))
    {
        return false;
    }
    int32_t from
        = __RingBuffer_map(ringBuffer, RingBuffer_size(ringBuffer) - 1);
    memcpy(dst,
           ringBuffer->buf + from * ringBuffer->eleSize,
           ringBuffer->eleSize);
    --ringBuffer->size;
    return true;
}

XCL_PUBLIC(void)
RingBuffer_clear(CRingBuffer* ringBuffer)
{
    ringBuffer->beg = ringBuffer->size = 0;
}

XCL_PUBLIC(void*)
RingBuffer_at(CRingBuffer* ringBuffer, int32_t pos)
{
    return ringBuffer->buf
           + __RingBuffer_map(ringBuffer, pos) * ringBuffer->eleSize;
}

XCL_PUBLIC(bool)
RingBuffer_get(const CRingBuffer* ringBuffer, int32_t pos, void* dst)
{
    if (!__RingBuffer_checkPosition(ringBuffer, false, pos))
    {
        return false;
    }
    int32_t from = __RingBuffer_map(ringBuffer, pos);
    memcpy(dst,
           ringBuffer->buf + from * ringBuffer->eleSize,
           ringBuffer->eleSize);
    return true;
}

bool
__RingBuffer_beforeInsert(CRingBuffer* ringBuffer,
                          int32_t pos,
                          int32_t len,
                          bool force)
{
    if ((!__RingBuffer_checkRegion(ringBuffer, true, pos, len) && !force))
    {
        return false;
    }
    if (len == 0)
    {
        return true;
    }
    int32_t size = RingBuffer_size(ringBuffer);
    if (pos < size - pos)
    {
        __RingBuffer_move(ringBuffer, -len, 0, pos);
        __RingBuffer_nextBeg(ringBuffer, -len);
    }
    else
    {
        __RingBuffer_move(ringBuffer, pos + len, pos, size - pos);
    }
    ringBuffer->size
        = size + len > ringBuffer->cap ? ringBuffer->cap : size + len;
    return true;
}

XCL_PUBLIC(bool)
RingBuffer_insert(CRingBuffer* ringBuffer,
                  int32_t pos,
                  const void* src,
                  bool force)
{
    if (__RingBuffer_beforeInsert(ringBuffer, pos, 1, force))
    {
        memcpy(RingBuffer_at(ringBuffer, pos), src, ringBuffer->eleSize);
        return true;
    }
    else
    {
        return false;
    }
}

XCL_PUBLIC(bool)
RingBuffer_insertRepeat(CRingBuffer* ringBuffer,
                        int32_t pos,
                        int32_t count,
                        const void* src,
                        bool force)
{
    if (__RingBuffer_beforeInsert(ringBuffer, pos, count, force))
    {
        for (int i = 0; i < count; i++)
        {
            memcpy(RingBuffer_at(ringBuffer, pos + i),
                   src,
                   ringBuffer->eleSize);
        }
        return true;
    }
    else
    {
        return false;
    }
}

XCL_PUBLIC(bool)
RingBuffer_insertRegion(CRingBuffer* ringBuffer,
                        int32_t pos,
                        const void* src,
                        int32_t es,
                        int32_t count,
                        bool force)
{
    if (es == ringBuffer->eleSize
        && __RingBuffer_beforeInsert(ringBuffer, pos, count, force))
    {
        __RingBuffer_copyExternalData(ringBuffer, pos, src, count);
        return true;
    }
    else
    {
        return false;
    }
}

bool
__RingBuffer_beforeWrite(CRingBuffer* ringBuffer, int32_t pos, int32_t n)
{
    if (n < 0 || !__RingBuffer_checkPosition(ringBuffer, false, pos))
    {
        return false;
    }
    if (n > ringBuffer->cap)
    {
        return false;
    }
    if (pos == 0)
        ringBuffer->beg = ringBuffer->size = 0;
    ringBuffer->size = n;
    return true;
}

XCL_PUBLIC(bool)
RingBuffer_assignRepeat(CRingBuffer* ringBuffer, int32_t n, const void* src)
{
    if (__RingBuffer_beforeWrite(ringBuffer, 0, n))
    {
        for (int i = 0; i < n; i++)
        {
            memcpy(RingBuffer_at(ringBuffer, i), src, ringBuffer->eleSize);
        }
        return true;
    }
    else
    {
        return false;
    }
}

XCL_PUBLIC(bool)
RingBuffer_assignRegion(CRingBuffer* ringBuffer,
                        const void* src,
                        int32_t es,
                        int32_t count)
{
    if (es != ringBuffer->eleSize)
    {
        return false;
    }
    if (__RingBuffer_beforeWrite(ringBuffer, 0, count))
    {
        __RingBuffer_copyExternalData(ringBuffer, 0, src, count);
        return true;
    }
    else
    {
        return false;
    }
}

bool
__RingBuffer_beforeRep(CRingBuffer* ringBuffer,
                       int32_t pos,
                       int32_t n,
                       int32_t len)
{
    if (!__RingBuffer_checkRegion(ringBuffer, false, pos, n))
    {
        return false;
    }
    if (len < 0)
    {
        return false;
    }
    int32_t size = ringBuffer->size;
    if (len != n)
    {
        if (pos < size - pos - n)
        {
            __RingBuffer_move(ringBuffer, n - len, 0, pos);
            __RingBuffer_nextBeg(ringBuffer, n - len);
        }
        else
        {
            __RingBuffer_move(ringBuffer, pos + len, pos + n, size - pos - n);
        }
        ringBuffer->size -= (n - len);
    }
    return true;
}

XCL_PUBLIC(bool)
RingBuffer_replace(CRingBuffer* ringBuffer,
                   int32_t pos,
                   int32_t n,
                   const void* src)
{
    return RingBuffer_replaceRepeat(ringBuffer, pos, n, 1, src);
}

XCL_PUBLIC(bool)
RingBuffer_replaceRepeat(CRingBuffer* ringBuffer,
                         int32_t pos,
                         int32_t n,
                         int32_t count,
                         const void* src)
{
    if (__RingBuffer_beforeRep(ringBuffer, pos, n, count))
    {
        for (int i = 0; i < count; i++)
        {
            memcpy(RingBuffer_at(ringBuffer, pos + i),
                   src,
                   ringBuffer->eleSize);
        }
        return true;
    }
    else
    {
        return false;
    }
}

XCL_PUBLIC(bool)
RingBuffer_replaceRegion(CRingBuffer* ringBuffer,
                         int32_t pos,
                         int32_t n,
                         const void* src,
                         int32_t es,
                         int32_t len)
{
    if (es != ringBuffer->eleSize)
    {
        return false;
    }
    if (__RingBuffer_beforeRep(ringBuffer, pos, n, len))
    {
        __RingBuffer_copyExternalData(ringBuffer, pos, src, len);
        return true;
    }
    else
    {
        return false;
    }
}

XCL_PUBLIC(bool)
RingBuffer_assignBufRegion(CRingBuffer* ringBuffer,
                           const CRingBuffer* src,
                           int32_t first,
                           int32_t last)
{
    if (src->eleSize != ringBuffer->eleSize)
    {
        return false;
    }
    if (__RingBuffer_beforeWrite(ringBuffer, 0, last - first))
    {
        __RingBuffer_copyExternalBuf(ringBuffer, 0, src, first, last - first);
        return true;
    }
    else
    {
        return false;
    }
}

XCL_PUBLIC(bool)
RingBuffer_assignBuf(CRingBuffer* ringBuffer, const CRingBuffer* src)
{
    return RingBuffer_assignBufRegion(ringBuffer, src, 0, RingBuffer_size(src));
}

XCL_PUBLIC(bool)
RingBuffer_insertBufRegion(CRingBuffer* ringBuffer,
                           int32_t pos,
                           const CRingBuffer* src,
                           int32_t first,
                           int32_t last,
                           bool force)
{
    if (ringBuffer->eleSize != src->eleSize)
    {
        return false;
    }
    if (__RingBuffer_beforeInsert(ringBuffer, pos, last - first, force))
    {
        __RingBuffer_copyExternalBuf(ringBuffer, pos, src, first, last - first);
        return true;
    }
    else
    {
        return false;
    }
}

XCL_PUBLIC(bool)
RingBuffer_replaceBufRegion(CRingBuffer* ringBuffer,
                            int32_t pos,
                            int32_t n,
                            const CRingBuffer* src,
                            int32_t first,
                            int32_t last)
{
    if (ringBuffer->eleSize != src->eleSize)
    {
        return false;
    }
    if (__RingBuffer_beforeRep(ringBuffer, pos, n, last - first))
    {
        __RingBuffer_copyExternalBuf(ringBuffer, pos, src, first, last - first);
        return true;
    }
    else
    {
        return false;
    }
}
