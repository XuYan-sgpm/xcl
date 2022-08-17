//
// Created by xuyan on 2022/7/13.
//

#include <string.h>
#include "util/ringbuffer.h"
#include "pool/pool.h"

XCL_EXPORT CRingBuffer XCL_API
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

XCL_EXPORT void XCL_API
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

XCL_EXPORT int32_t XCL_API
RingBuffer_size(const CRingBuffer* ringBuffer)
{
    return ringBuffer->size;
}

XCL_EXPORT int32_t XCL_API
RingBuffer_capacity(const CRingBuffer* ringBuffer)
{
    return ringBuffer->cap;
}

XCL_EXPORT bool XCL_API
RingBuffer_empty(const CRingBuffer* ringBuffer)
{
    return ringBuffer->size == 0;
}

static bool
__RingBuffer_checkPosition(const CRingBuffer* ringBuffer,
                           bool read,
                           int32_t pos)
{
    return pos >= 0
           && (read ? pos < RingBuffer_size(ringBuffer)
                    : pos <= RingBuffer_size(ringBuffer));
}

static bool
__RingBuffer_checkRegion(const CRingBuffer* ringBuffer,
                         bool read,
                         int32_t pos,
                         int32_t len)
{
    int32_t size = RingBuffer_size(ringBuffer);
    if (!read)
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

static int32_t
__RingBuffer_write(const CRingBuffer* ringBuffer,
                   int32_t srcIdx,
                   void* dst,
                   int32_t len)
{
    int32_t total = 0;
    int32_t es = ringBuffer->eleSize;
    int32_t from = __RingBuffer_map(ringBuffer, srcIdx);
    while (total < len)
    {
        int32_t copied = ringBuffer->cap - from;
        if (copied > len - total)
            copied = len - total;
        memcpy(dst, ringBuffer->buf + from * es, copied * es);
        from = __RingBuffer_mappedNext(ringBuffer, from, copied);
        total += copied;
    }
    return total;
}

static int32_t
__RingBuffer_writeStream(const CRingBuffer* ringBuffer,
                         int32_t srcIdx,
                         int32_t n,
                         int32_t (*write)(void*, const void*, int32_t),
                         void* usr)
{
    int32_t total = 0;
    int32_t es = ringBuffer->eleSize;
    int32_t from = __RingBuffer_map(ringBuffer, srcIdx);
    while (total < n * es)
    {
        int32_t copied = ringBuffer->cap - from;
        if (copied > n - total)
            copied = n - total;
        for (int32_t written = 0; written < copied * es;)
        {
            int32_t ret = write(usr,
                                ringBuffer->buf + from * es,
                                copied * es - written);
            if (ret < 0)
            {
                return total;
            }
            written += ret;
            total += ret;
        }
        from = __RingBuffer_mappedNext(ringBuffer, from, copied);
    }
    return total;
}

static inline void
__RingBuffer_incSize(CRingBuffer* ringBuffer, int32_t delta)
{
    int32_t size = ringBuffer->size;
    if (size + delta >= 0 && size + delta <= ringBuffer->cap)
    {
        size += delta;
    }
    else
    {
        return;
    }
    ringBuffer->size = size;
}

XCL_EXPORT bool XCL_API
RingBuffer_pushFront(CRingBuffer* ringBuffer, const void* src, bool force)
{
    if (!__RingBuffer_checkRegion(ringBuffer, false, 0, 1) && !force)
    {
        return false;
    }
    int32_t to = __RingBuffer_map(ringBuffer, -1);
    memcpy(ringBuffer->buf + to * ringBuffer->eleSize,
           src,
           ringBuffer->eleSize);
    __RingBuffer_nextBeg(ringBuffer, -1);
    __RingBuffer_incSize(ringBuffer, 1);
    return true;
}

XCL_EXPORT bool XCL_API
RingBuffer_pushBack(CRingBuffer* ringBuffer, const void* src, bool force)
{
    if (!__RingBuffer_checkRegion(ringBuffer,
                                  false,
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
    __RingBuffer_incSize(ringBuffer, 1);
    return true;
}

XCL_EXPORT bool XCL_API
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

XCL_EXPORT bool XCL_API
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

XCL_EXPORT void XCL_API
RingBuffer_clear(CRingBuffer* ringBuffer)
{
    ringBuffer->beg = ringBuffer->size = 0;
}

XCL_EXPORT void* XCL_API
RingBuffer_at(CRingBuffer* ringBuffer, int32_t pos)
{
    return ringBuffer->buf
           + __RingBuffer_map(ringBuffer, pos) * ringBuffer->eleSize;
}

XCL_EXPORT bool XCL_API
RingBuffer_get(const CRingBuffer* ringBuffer, int32_t pos, void* dst)
{
    if (!__RingBuffer_checkPosition(ringBuffer, true, pos))
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
    if ((!__RingBuffer_checkRegion(ringBuffer, false, pos, len) && !force))
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
    __RingBuffer_incSize(ringBuffer, len);
    return true;
}

XCL_EXPORT bool XCL_API
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

XCL_EXPORT bool XCL_API
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

XCL_EXPORT bool XCL_API
RingBuffer_insertRegion(CRingBuffer* ringBuffer,
                        int32_t pos,
                        const void* src,
                        int32_t es,
                        int32_t len,
                        bool force)
{
    if (es == ringBuffer->eleSize
        && __RingBuffer_beforeInsert(ringBuffer, pos, len, force))
    {
        __RingBuffer_copyExternalData(ringBuffer, pos, src, len);
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
        ringBuffer->beg = 0;
    ringBuffer->size = n;
    return true;
}

XCL_EXPORT bool XCL_API
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

XCL_EXPORT bool XCL_API
RingBuffer_assignRegion(CRingBuffer* ringBuffer,
                        const void* src,
                        int32_t es,
                        int32_t len)
{
    if (es != ringBuffer->eleSize)
    {
        return false;
    }
    if (__RingBuffer_beforeWrite(ringBuffer, 0, len))
    {
        __RingBuffer_copyExternalData(ringBuffer, 0, src, len);
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
    if (!__RingBuffer_checkRegion(ringBuffer, true, pos, n))
    {
        return false;
    }
    if (len < 0)
    {
        return false;
    }
    int32_t size = ringBuffer->size;
    if (len - n > 0 && len - n > ringBuffer->cap - size)
    {
        return false;
    }
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

XCL_EXPORT bool XCL_API
RingBuffer_replace(CRingBuffer* ringBuffer,
                   int32_t pos,
                   int32_t n,
                   const void* src)
{
    return RingBuffer_replaceRepeat(ringBuffer, pos, n, 1, src);
}

XCL_EXPORT bool XCL_API
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

XCL_EXPORT bool XCL_API
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

XCL_EXPORT bool XCL_API
RingBuffer_assignBufRegion(CRingBuffer* ringBuffer,
                           const CRingBuffer* src,
                           int32_t first,
                           int32_t last)
{
    if (src->eleSize != ringBuffer->eleSize)
    {
        return false;
    }
    if (!__RingBuffer_checkRegion(src, true, first, last - first))
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

XCL_EXPORT bool XCL_API
RingBuffer_assignBuf(CRingBuffer* ringBuffer, const CRingBuffer* src)
{
    return RingBuffer_assignBufRegion(ringBuffer, src, 0, RingBuffer_size(src));
}

XCL_EXPORT bool XCL_API
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
    if (!__RingBuffer_checkRegion(src, true, first, last - first))
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

XCL_EXPORT bool XCL_API
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
    if (!__RingBuffer_checkRegion(src, true, first, last - first))
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

XCL_EXPORT int32_t XCL_API
RingBuffer_writeStream(const CRingBuffer* ringBuffer,
                       int32_t pos,
                       int32_t n,
                       int32_t (*write)(void*, const void*, int32_t),
                       void* usr)
{
    if (!__RingBuffer_checkRegion(ringBuffer, true, pos, n))
    {
        return -1;
    }
    return __RingBuffer_writeStream(ringBuffer, pos, n, write, usr);
}

XCL_EXPORT int32_t XCL_API
RingBuffer_write(const CRingBuffer* ringBuffer,
                 int32_t pos,
                 void* dst,
                 int32_t len)
{
    if (!__RingBuffer_checkPosition(ringBuffer, true, pos))
    {
        return -1;
    }
    if (len == 0)
    {
        return 0;
    }
    if (len < 0)
    {
        return -1;
    }
    if (len > ringBuffer->size - pos)
    {
        len = ringBuffer->size - pos;
    }
    return __RingBuffer_write(ringBuffer, pos, dst, len);
}

XCL_EXPORT void XCL_API
RingBuffer_remove(CRingBuffer* ringBuffer, int32_t pos, int32_t n)
{
    if (!__RingBuffer_checkRegion(ringBuffer, true, pos, n))
    {
        return;
    }
    if (pos == 0)
    {
        if (n < ringBuffer->size)
        {
            __RingBuffer_nextBeg(ringBuffer, n);
        }
        else
        {
            ringBuffer->beg = 0;
        }
    }
    else if (pos + n == ringBuffer->size)
    {
    }
    else
    {
        if (pos < ringBuffer->size - pos - n)
        {
            __RingBuffer_move(ringBuffer, n, 0, pos);
            __RingBuffer_nextBeg(ringBuffer, n);
        }
        else
        {
            __RingBuffer_move(ringBuffer,
                              pos,
                              pos + n,
                              ringBuffer->size - pos - n);
        }
    }
    ringBuffer->size -= n;
}
