//
// Created by xuyan on 2022/7/13.
//

#include <string.h>
#include "util/ring_buffer.h"
#include "pool/pool.h"

RingBuffer XCL_API
RingBuffer_make(int32_t cap, int32_t es)
{
    char* p = Pool_alloc(Pool_global(), ((int64_t)(cap)) * es);
    if (p)
    {
        return (RingBuffer){p, 0, 0, cap, es};
    }
    else
    {
        return (RingBuffer){p, 0, 0, 0, 0};
    }
}

void XCL_API
RingBuffer_release(RingBuffer* ring_buffer)
{
    if (ring_buffer->buf)
    {
        Pool_dealloc(Pool_global(),
                     ring_buffer->buf,
                     ((int64_t)(ring_buffer->cap)) * ring_buffer->element_size);
    }
    memset(ring_buffer, 0, sizeof(RingBuffer));
}

int32_t XCL_API
RingBuffer_size(const RingBuffer* ring_buffer)
{
    return ring_buffer->size;
}

int32_t XCL_API
RingBuffer_capacity(const RingBuffer* ring_buffer)
{
    return ring_buffer->cap;
}

bool XCL_API
RingBuffer_empty(const RingBuffer* ring_buffer)
{
    return ring_buffer->size == 0;
}

static bool
__RingBuffer_checkPosition(const RingBuffer* ring_buffer,
                           bool read,
                           int32_t pos)
{
    return pos >= 0 && (read ? pos < RingBuffer_size(ring_buffer)
                             : pos <= RingBuffer_size(ring_buffer));
}

static bool
__RingBuffer_checkRegion(const RingBuffer* ring_buffer,
                         bool read,
                         int32_t pos,
                         int32_t len)
{
    int32_t size = RingBuffer_size(ring_buffer);
    if (!read)
    {
        if (pos < 0 || len < 0 || pos > size)
        {
            return false;
        }
        if (len - (RingBuffer_capacity(ring_buffer) - size) > 0)
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
__RingBuffer_offset(const RingBuffer* ring_buffer, int32_t off)
{
    off = off >= 0 ? off % ring_buffer->cap : -((-off) % ring_buffer->cap);
    return off;
}

static inline int32_t
__RingBuffer_map(const RingBuffer* ring_buffer, int32_t pos)
{
    return (ring_buffer->beg + ring_buffer->cap +
            __RingBuffer_offset(ring_buffer, pos)) %
           ring_buffer->cap;
}

static inline int32_t
__RingBuffer_mappedNext(const RingBuffer* ring_buffer,
                        int32_t mappedPos,
                        int32_t step)
{

    return (mappedPos + __RingBuffer_offset(ring_buffer, step)) %
           (ring_buffer->cap);
}

static inline void
__RingBuffer_nextBeg(RingBuffer* ring_buffer, int32_t step)
{
    ring_buffer->beg = __RingBuffer_map(ring_buffer, step);
}

static void
__RingBuffer_move(RingBuffer* ring_buffer,
                  int32_t dst_idx,
                  int32_t src_idx,
                  int32_t len)
{
    if (dst_idx == src_idx || len == 0)
    {
        return;
    }
    int32_t total = 0;
    int32_t es = ring_buffer->element_size;
    char* buf = ring_buffer->buf;
    if (dst_idx < src_idx)
    {
        int32_t to = __RingBuffer_map(ring_buffer, dst_idx);
        int32_t from = __RingBuffer_map(ring_buffer, src_idx);
        while (total < len)
        {
            int32_t copied = ring_buffer->cap - (to > from ? to : from);
            if (copied > len - total)
                copied = len - total;
            memcpy(buf + to * es, buf + from * es, copied * es);
            to = __RingBuffer_mappedNext(ring_buffer, to, copied);
            from = __RingBuffer_mappedNext(ring_buffer, from, copied);
            total += copied;
        }
    }
    else
    {
        int32_t to = __RingBuffer_map(ring_buffer, dst_idx + len);
        int32_t from = __RingBuffer_map(ring_buffer, src_idx + len);
        while (total < len)
        {
            if (to == 0)
                to = ring_buffer->cap;
            if (from == 0)
                from = ring_buffer->cap;
            int32_t copied = to > from ? from : to;
            if (copied > len - total)
                copied = len - total;
            memmove(buf + (to - copied) * es,
                    buf + (from - copied) * es,
                    copied * es);
            to = __RingBuffer_mappedNext(ring_buffer, to, -copied);
            from = __RingBuffer_mappedNext(ring_buffer, from, -copied);
            total += copied;
        }
    }
}

static void
__RingBuffer_copyExternalData(RingBuffer* ring_buffer,
                              int32_t dst_idx,
                              const void* src,
                              int32_t len)
{
    if (len == 0)
    {
        return;
    }
    int32_t total = 0;
    int32_t es = ring_buffer->element_size;
    char* buf = ring_buffer->buf;
    int32_t to = __RingBuffer_map(ring_buffer, dst_idx);
    while (total < len)
    {
        int32_t copied = ring_buffer->cap - to;
        if (copied > len - total)
            copied = len - total;
        memcpy(buf + to * es, src, copied * es);
        to = __RingBuffer_mappedNext(ring_buffer, to, copied);
        src = (const char*)src + copied * es;
        total += copied;
    }
}

static void
__RingBuffer_copyExternalBuf(RingBuffer* ring_buffer,
                             int32_t dst_idx,
                             const RingBuffer* src,
                             int32_t src_idx,
                             int32_t len)
{
    if (len == 0)
    {
        return;
    }
    int32_t total = 0;
    int32_t es = ring_buffer->element_size;
    int32_t to = __RingBuffer_map(ring_buffer, dst_idx);
    int32_t from = __RingBuffer_map(src, src_idx);
    while (total < len)
    {
        int32_t copied = ring_buffer->cap - to;
        if (copied > src->cap - from)
            copied = src->cap - from;
        if (copied > len - total)
            copied = len - total;
        memcpy(ring_buffer->buf + to * es, src->buf + from * es, copied * es);
        to = __RingBuffer_mappedNext(ring_buffer, to, copied);
        from = __RingBuffer_mappedNext(src, from, copied);
        total += copied;
    }
}

static int32_t
__RingBuffer_write(const RingBuffer* ring_buffer,
                   int32_t src_idx,
                   void* dst,
                   int32_t len)
{
    int32_t total = 0;
    int32_t es = ring_buffer->element_size;
    int32_t from = __RingBuffer_map(ring_buffer, src_idx);
    while (total < len)
    {
        int32_t copied = ring_buffer->cap - from;
        if (copied > len - total)
            copied = len - total;
        memcpy(dst, ring_buffer->buf + from * es, copied * es);
        from = __RingBuffer_mappedNext(ring_buffer, from, copied);
        total += copied;
    }
    return total;
}

static int32_t
__RingBuffer_writeStream(const RingBuffer* ring_buffer,
                         int32_t src_idx,
                         int32_t n,
                         int32_t (*write)(void*, const void*, int32_t),
                         void* obj)
{
    int32_t total = 0;
    int32_t es = ring_buffer->element_size;
    int32_t from = __RingBuffer_map(ring_buffer, src_idx);
    while (total < n * es)
    {
        int32_t copied = ring_buffer->cap - from;
        if (copied > n - total)
            copied = n - total;
        for (int32_t written = 0; written < copied * es;)
        {
            int32_t ret =
                write(obj, ring_buffer->buf + from * es, copied * es - written);
            if (ret < 0)
            {
                return total;
            }
            written += ret;
            total += ret;
        }
        from = __RingBuffer_mappedNext(ring_buffer, from, copied);
    }
    return total;
}

static inline void
__RingBuffer_incSize(RingBuffer* ring_buffer, int32_t delta)
{
    int32_t size = ring_buffer->size;
    if (size + delta >= 0 && size + delta <= ring_buffer->cap)
    {
        size += delta;
    }
    else
    {
        return;
    }
    ring_buffer->size = size;
}

bool XCL_API
RingBuffer_pushFront(RingBuffer* ring_buffer, const void* src, bool force)
{
    if (!__RingBuffer_checkRegion(ring_buffer, false, 0, 1) && !force)
    {
        return false;
    }
    int32_t to = __RingBuffer_map(ring_buffer, -1);
    memcpy(ring_buffer->buf + to * ring_buffer->element_size,
           src,
           ring_buffer->element_size);
    __RingBuffer_nextBeg(ring_buffer, -1);
    __RingBuffer_incSize(ring_buffer, 1);
    return true;
}

bool XCL_API
RingBuffer_pushBack(RingBuffer* ring_buffer, const void* src, bool force)
{
    if (!__RingBuffer_checkRegion(ring_buffer,
                                  false,
                                  RingBuffer_size(ring_buffer),
                                  1) &&
        !force)
    {
        return false;
    }
    int32_t to = __RingBuffer_map(ring_buffer, RingBuffer_size(ring_buffer));
    memcpy(ring_buffer->buf + to * ring_buffer->element_size,
           src,
           ring_buffer->element_size);
    __RingBuffer_incSize(ring_buffer, 1);
    return true;
}

bool XCL_API
RingBuffer_popFront(RingBuffer* ring_buffer, void* dst)
{
    if (RingBuffer_empty(ring_buffer))
    {
        return false;
    }
    int32_t from = __RingBuffer_map(ring_buffer, 0);
    memcpy(dst,
           ring_buffer->buf + from * ring_buffer->element_size,
           ring_buffer->element_size);
    __RingBuffer_nextBeg(ring_buffer, 1);
    --ring_buffer->size;
    return true;
}

bool XCL_API
RingBuffer_popBack(RingBuffer* ring_buffer, void* dst)
{
    if (RingBuffer_empty(ring_buffer))
    {
        return false;
    }
    int32_t from =
        __RingBuffer_map(ring_buffer, RingBuffer_size(ring_buffer) - 1);
    memcpy(dst,
           ring_buffer->buf + from * ring_buffer->element_size,
           ring_buffer->element_size);
    --ring_buffer->size;
    return true;
}

void XCL_API
RingBuffer_clear(RingBuffer* ring_buffer)
{
    ring_buffer->beg = ring_buffer->size = 0;
}

void* XCL_API
RingBuffer_at(RingBuffer* ring_buffer, int32_t pos)
{
    return ring_buffer->buf +
           __RingBuffer_map(ring_buffer, pos) * ring_buffer->element_size;
}

bool XCL_API
RingBuffer_get(const RingBuffer* ring_buffer, int32_t pos, void* dst)
{
    if (!__RingBuffer_checkPosition(ring_buffer, true, pos))
    {
        return false;
    }
    int32_t from = __RingBuffer_map(ring_buffer, pos);
    memcpy(dst,
           ring_buffer->buf + from * ring_buffer->element_size,
           ring_buffer->element_size);
    return true;
}

bool
__RingBuffer_insertAux(RingBuffer* ring_buffer,
                       int32_t pos,
                       int32_t len,
                       bool force)
{
    if ((!__RingBuffer_checkRegion(ring_buffer, false, pos, len) && !force))
    {
        return false;
    }
    if (len == 0)
    {
        return true;
    }
    int32_t size = RingBuffer_size(ring_buffer);
    if (pos < size - pos)
    {
        __RingBuffer_move(ring_buffer, -len, 0, pos);
        __RingBuffer_nextBeg(ring_buffer, -len);
    }
    else
    {
        __RingBuffer_move(ring_buffer, pos + len, pos, size - pos);
    }
    __RingBuffer_incSize(ring_buffer, len);
    return true;
}

bool XCL_API
RingBuffer_insert(RingBuffer* ring_buffer,
                  int32_t pos,
                  const void* src,
                  bool force)
{
    if (__RingBuffer_insertAux(ring_buffer, pos, 1, force))
    {
        memcpy(RingBuffer_at(ring_buffer, pos), src, ring_buffer->element_size);
        return true;
    }
    else
    {
        return false;
    }
}

bool XCL_API
RingBuffer_insertRepeat(RingBuffer* ring_buffer,
                        int32_t pos,
                        int32_t count,
                        const void* src,
                        bool force)
{
    if (__RingBuffer_insertAux(ring_buffer, pos, count, force))
    {
        for (int i = 0; i < count; i++)
        {
            memcpy(RingBuffer_at(ring_buffer, pos + i),
                   src,
                   ring_buffer->element_size);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool XCL_API
RingBuffer_insertRegion(RingBuffer* ring_buffer,
                        int32_t pos,
                        const void* src,
                        int32_t es,
                        int32_t len,
                        bool force)
{
    if (es == ring_buffer->element_size &&
        __RingBuffer_insertAux(ring_buffer, pos, len, force))
    {
        __RingBuffer_copyExternalData(ring_buffer, pos, src, len);
        return true;
    }
    else
    {
        return false;
    }
}

bool
__RingBuffer_writeAux(RingBuffer* ring_buffer, int32_t pos, int32_t n)
{
    if (n < 0 || !__RingBuffer_checkPosition(ring_buffer, false, pos))
    {
        return false;
    }
    if (n > ring_buffer->cap)
    {
        return false;
    }
    if (pos == 0)
        ring_buffer->beg = 0;
    ring_buffer->size = n;
    return true;
}

bool XCL_API
RingBuffer_assignRepeat(RingBuffer* ring_buffer, int32_t n, const void* src)
{
    if (__RingBuffer_writeAux(ring_buffer, 0, n))
    {
        for (int i = 0; i < n; i++)
        {
            memcpy(RingBuffer_at(ring_buffer, i),
                   src,
                   ring_buffer->element_size);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool XCL_API
RingBuffer_assignRegion(RingBuffer* ring_buffer,
                        const void* src,
                        int32_t es,
                        int32_t len)
{
    if (es != ring_buffer->element_size)
    {
        return false;
    }
    if (__RingBuffer_writeAux(ring_buffer, 0, len))
    {
        __RingBuffer_copyExternalData(ring_buffer, 0, src, len);
        return true;
    }
    else
    {
        return false;
    }
}

bool
__RingBuffer_replaceAux(RingBuffer* ring_buffer,
                        int32_t pos,
                        int32_t n,
                        int32_t len)
{
    if (!__RingBuffer_checkRegion(ring_buffer, true, pos, n))
    {
        return false;
    }
    if (len < 0)
    {
        return false;
    }
    int32_t size = ring_buffer->size;
    if (len - n > 0 && len - n > ring_buffer->cap - size)
    {
        return false;
    }
    if (len != n)
    {
        if (pos < size - pos - n)
        {
            __RingBuffer_move(ring_buffer, n - len, 0, pos);
            __RingBuffer_nextBeg(ring_buffer, n - len);
        }
        else
        {
            __RingBuffer_move(ring_buffer, pos + len, pos + n, size - pos - n);
        }
        ring_buffer->size -= (n - len);
    }
    return true;
}

bool XCL_API
RingBuffer_replace(RingBuffer* ring_buffer,
                   int32_t pos,
                   int32_t n,
                   const void* src)
{
    return RingBuffer_replaceRepeat(ring_buffer, pos, n, 1, src);
}

bool XCL_API
RingBuffer_replaceRepeat(RingBuffer* ring_buffer,
                         int32_t pos,
                         int32_t n,
                         int32_t count,
                         const void* src)
{
    if (__RingBuffer_replaceAux(ring_buffer, pos, n, count))
    {
        for (int i = 0; i < count; i++)
        {
            memcpy(RingBuffer_at(ring_buffer, pos + i),
                   src,
                   ring_buffer->element_size);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool XCL_API
RingBuffer_replaceRegion(RingBuffer* ring_buffer,
                         int32_t pos,
                         int32_t n,
                         const void* src,
                         int32_t es,
                         int32_t len)
{
    if (es != ring_buffer->element_size)
    {
        return false;
    }
    if (__RingBuffer_replaceAux(ring_buffer, pos, n, len))
    {
        __RingBuffer_copyExternalData(ring_buffer, pos, src, len);
        return true;
    }
    else
    {
        return false;
    }
}

bool XCL_API
RingBuffer_assignBufRegion(RingBuffer* ring_buffer,
                           const RingBuffer* src,
                           int32_t first,
                           int32_t last)
{
    if (src->element_size != ring_buffer->element_size)
    {
        return false;
    }
    if (!__RingBuffer_checkRegion(src, true, first, last - first))
    {
        return false;
    }
    if (__RingBuffer_writeAux(ring_buffer, 0, last - first))
    {
        __RingBuffer_copyExternalBuf(ring_buffer, 0, src, first, last - first);
        return true;
    }
    else
    {
        return false;
    }
}

bool XCL_API
RingBuffer_assignBuf(RingBuffer* ring_buffer, const RingBuffer* src)
{
    return RingBuffer_assignBufRegion(ring_buffer,
                                      src,
                                      0,
                                      RingBuffer_size(src));
}

bool XCL_API
RingBuffer_insertBufRegion(RingBuffer* ring_buffer,
                           int32_t pos,
                           const RingBuffer* src,
                           int32_t first,
                           int32_t last,
                           bool force)
{
    if (ring_buffer->element_size != src->element_size)
    {
        return false;
    }
    if (!__RingBuffer_checkRegion(src, true, first, last - first))
    {
        return false;
    }
    if (__RingBuffer_insertAux(ring_buffer, pos, last - first, force))
    {
        __RingBuffer_copyExternalBuf(ring_buffer,
                                     pos,
                                     src,
                                     first,
                                     last - first);
        return true;
    }
    else
    {
        return false;
    }
}

bool XCL_API
RingBuffer_replaceBufRegion(RingBuffer* ring_buffer,
                            int32_t pos,
                            int32_t n,
                            const RingBuffer* src,
                            int32_t first,
                            int32_t last)
{
    if (ring_buffer->element_size != src->element_size)
    {
        return false;
    }
    if (!__RingBuffer_checkRegion(src, true, first, last - first))
    {
        return false;
    }
    if (__RingBuffer_replaceAux(ring_buffer, pos, n, last - first))
    {
        __RingBuffer_copyExternalBuf(ring_buffer,
                                     pos,
                                     src,
                                     first,
                                     last - first);
        return true;
    }
    else
    {
        return false;
    }
}

int32_t XCL_API
RingBuffer_writeStream(const RingBuffer* ring_buffer,
                       int32_t pos,
                       int32_t n,
                       int32_t (*write)(void*, const void*, int32_t),
                       void* obj)
{
    if (!__RingBuffer_checkRegion(ring_buffer, true, pos, n))
    {
        return -1;
    }
    return __RingBuffer_writeStream(ring_buffer, pos, n, write, obj);
}

int32_t XCL_API
RingBuffer_write(const RingBuffer* ring_buffer,
                 int32_t pos,
                 void* dst,
                 int32_t len)
{
    if (!__RingBuffer_checkPosition(ring_buffer, true, pos))
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
    if (len > ring_buffer->size - pos)
    {
        len = ring_buffer->size - pos;
    }
    return __RingBuffer_write(ring_buffer, pos, dst, len);
}

void XCL_API
RingBuffer_remove(RingBuffer* ring_buffer, int32_t pos, int32_t n)
{
    if (!__RingBuffer_checkRegion(ring_buffer, true, pos, n))
    {
        return;
    }
    if (pos == 0)
    {
        if (n < ring_buffer->size)
        {
            __RingBuffer_nextBeg(ring_buffer, n);
        }
        else
        {
            ring_buffer->beg = 0;
        }
    }
    else if (pos + n == ring_buffer->size)
    {
    }
    else
    {
        if (pos < ring_buffer->size - pos - n)
        {
            __RingBuffer_move(ring_buffer, n, 0, pos);
            __RingBuffer_nextBeg(ring_buffer, n);
        }
        else
        {
            __RingBuffer_move(ring_buffer,
                              pos,
                              pos + n,
                              ring_buffer->size - pos - n);
        }
    }
    ring_buffer->size -= n;
}
