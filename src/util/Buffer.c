#include "util/buffer.h"
#include "pool/pool.h"
#include <string.h>

static const unsigned __BUF_MASK__ = 0x7fffffff;

static inline bool
__Buffer_isReleasable(Buffer* buffer)
{
    return (buffer->state & ~__BUF_MASK__) >> 31;
}

static inline void
__Buffer_setState(Buffer* buffer, int cap, const bool flag)
{
    int val = flag;
    buffer->state = (cap & __BUF_MASK__) | (val << 31);
}

static inline void
__Buffer_dealloc(Buffer* buffer)
{
    if (buffer->data)
        Pool_dealloc(Pool_def(), buffer->data, Buffer_cap(buffer));
    memset(buffer, 0, sizeof(Buffer));
}

static inline bool
__Buffer_alloc(Buffer* buffer, int32_t cap)
{
    buffer->data = Pool_alloc(Pool_def(), cap);
    if (buffer->data)
    {
        buffer->size = 0;
        __Buffer_setState(buffer, cap, true);
    }
    return buffer->data;
}

static inline bool
__Buffer_reapply(Buffer* buffer, int32_t req)
{
    void* p = Pool_reapply(Pool_def(), buffer->data, Buffer_cap(buffer), req);
    if (p)
    {
        buffer->data = p;
        __Buffer_setState(buffer, req, true);
    }
    return p;
}

XCL_EXPORT Buffer XCL_API
Buffer_make(int cap)
{
    Buffer buffer = {NULL, 0, 0};
    if (cap > 0)
    {
        __Buffer_alloc(&buffer, cap);
    }
    return buffer;
}

XCL_EXPORT Buffer XCL_API
Buffer_newRegion(char* src, int len)
{
    Buffer buffer = {NULL, 0, 0};
    if (src && len > 0)
    {
        buffer.data = src;
        __Buffer_setState(&buffer, len, false);
    }
    return buffer;
}

XCL_EXPORT int XCL_API
Buffer_cap(const Buffer* buffer)
{
    return (int)(buffer->state & __BUF_MASK__);
}

XCL_EXPORT Buffer XCL_API
wrapBuf(const char* src, int len)
{
    Buffer buffer = {(char*)src, 0, len};
    __Buffer_setState(&buffer, len, false);
    return buffer;
}

XCL_EXPORT Buffer XCL_API
wrapBuf2(const Buffer* origin, int pos, int len)
{
    Buffer buffer = {NULL, 0, 0};
    if (origin && pos >= 0 && len >= 0 && pos + len <= origin->size)
    {
        buffer.size = len;
        buffer.data = &origin->data[pos];
        __Buffer_setState(&buffer, len, false);
    }
    return buffer;
}

XCL_EXPORT bool XCL_API
Buffer_release(Buffer* buffer)
{
    if (buffer->data && __Buffer_isReleasable(buffer))
    {
        __Buffer_dealloc(buffer);
        return true;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Buffer_push(Buffer* buffer, char ch)
{
    if (buffer->size < Buffer_cap(buffer))
    {
        buffer->data[buffer->size++] = ch;
        return true;
    }
    return false;
}

XCL_EXPORT int XCL_API
Buffer_appendRegion(Buffer* buffer, const char* src, int len)
{
    if (len <= 0 || !src)
    {
        return 0;
    }
    int write = Buffer_cap(buffer) - buffer->size;
    if (write > len)
    {
        write = len;
    }
    memcpy(buffer->data + buffer->size, src, write);
    buffer->size += write;
    return write;
}

XCL_EXPORT int XCL_API
Buffer_append(Buffer* buffer, const char* src)
{
    return src ? Buffer_appendRegion(buffer, src, strlen(src)) : 0;
}

XCL_EXPORT int XCL_API
Buffer_appendChars(Buffer* buffer, int n, char ch)
{
    if (n <= 0)
    {
        return 0;
    }
    int count = Buffer_cap(buffer) - buffer->size;
    if (count > n)
    {
        count = n;
    }
    memset(buffer->data + buffer->size, count, ch);
    buffer->size += count;
    return count;
}

XCL_EXPORT bool XCL_API
Buffer_pop(Buffer* buffer, char* dst)
{
    if (buffer->size)
    {
        if (dst)
        {
            *dst = buffer->data[0];
        }
        if (buffer->size > 1)
        {
            memcpy(buffer->data, buffer->data + 1, buffer->size - 1);
        }
        --buffer->size;
        return true;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Buffer_get(const Buffer* buffer, int pos, char* dst)
{
    if (pos < 0 || pos >= buffer->size)
    {
        return false;
    }
    if (dst)
    {
        *dst = buffer->data[pos];
    }
    return true;
}

XCL_EXPORT bool XCL_API
Buffer_writeChar(Buffer* buffer, int pos, char ch)
{
    if (pos >= 0 && pos <= buffer->size && buffer->size < Buffer_cap(buffer))
    {
        if (pos < buffer->size)
        {
            memmove(buffer->data + pos + 1,
                    buffer->data + pos,
                    buffer->size - pos);
        }
        buffer->data[pos] = ch;
        ++buffer->size;
        return true;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Buffer_writeChars(Buffer* buffer, int pos, int n, char ch)
{
    if (pos >= 0 && n >= 0 && buffer->size + n <= Buffer_cap(buffer))
    {
        if (pos < buffer->size)
        {
            memmove(buffer->data + pos + n,
                    buffer->data + pos,
                    buffer->size - pos);
        }
        memset(buffer->data + pos, n, ch);
        buffer->size += n;
        return true;
    }
    return false;
}

XCL_EXPORT int XCL_API
Buffer_writeRegion(Buffer* buffer, int pos, const char* src, int len)
{
    if (pos > buffer->size || len <= 0 || !src)
    {
        return 0;
    }
    int remaining = Buffer_cap(buffer) - buffer->size;
    int write = remaining > len ? len : remaining;
    if (pos < buffer->size)
    {
        memmove(&buffer->data[pos + write],
                &buffer->data[pos],
                buffer->size - pos);
    }
    memcpy(buffer->data + pos, src, len);
    buffer->size += len;
    return write;
}

XCL_EXPORT int XCL_API
Buffer_write(Buffer* buffer, int pos, const char* src)
{
    return src ? Buffer_writeRegion(buffer, pos, src, strlen(src)) : 0;
}

XCL_EXPORT int XCL_API
Buffer_readRegion(const Buffer* buffer, int pos, char* dst, int len)
{
    if (pos >= buffer->size || len <= 0)
    {
        return 0;
    }
    int available = buffer->size - pos;
    int read = available > len ? len : available;
    if (read > 1)
    {
        memcpy(dst, buffer->data + pos, read);
    }
    else
    {
        *dst = buffer->data[pos];
    }
    return read;
}

XCL_EXPORT int XCL_API
Buffer_read(const Buffer* buffer, int pos, char* dst)
{
    if (buffer->size - pos > 0)
    {
        return Buffer_readRegion(buffer, pos, dst, buffer->size - pos);
    }
    return 0;
}

XCL_EXPORT bool XCL_API
Buffer_expand(Buffer* buffer, int cap)
{
    if (cap <= Buffer_cap(buffer))
    {
    }
    else
    {
        if (!buffer->data || !__Buffer_isReleasable(buffer))
        {
            Buffer newBuf = Buffer_make(cap);
            if (!newBuf.data)
            {
                return false;
            }
            *buffer = newBuf;
        }
        else
        {
            if (!__Buffer_reapply(buffer, cap))
            {
                Buffer newBuf = Buffer_make(cap);
                if (newBuf.data)
                {
                    memcpy(newBuf.data, buffer->data, buffer->size);
                    newBuf.size = buffer->size;
                    __Buffer_dealloc(buffer);
                }
                else
                {
                    return false;
                }
                *buffer = newBuf;
            }
        }
    }
    return true;
}

XCL_EXPORT void XCL_API
Buffer_removeRegion(Buffer* buffer, int pos, int len)
{
    if (pos >= 0 && len >= 0 && pos + len <= buffer->size)
    {
        if (pos + len < buffer->size)
        {
            memcpy(buffer->data + pos,
                   buffer->data + (pos + len),
                   buffer->size - pos - len);
        }
        buffer->size -= len;
    }
}

XCL_EXPORT void XCL_API
Buffer_removePos(Buffer* buffer, int pos)
{
    Buffer_removeRegion(buffer, pos, 1);
}

XCL_EXPORT void XCL_API
Buffer_clear(Buffer* buffer)
{
    if (buffer->size > 0)
    {
        buffer->size = 0;
    }
}

XCL_EXPORT bool XCL_API
Buffer_remake(Buffer* buffer, int cap)
{
    if (cap < 0)
    {
        return false;
    }
    if (cap == 0)
    {
        return Buffer_release(buffer);
    }
    else
    {
        if (!__Buffer_isReleasable(buffer))
        {
            Buffer newBuf = Buffer_make(cap);
            if (!newBuf.data)
            {
                return false;
            }
            int size = cap > buffer->size ? buffer->size : cap;
            memcpy(newBuf.data, buffer->data, size);
            newBuf.size = size;
            *buffer = newBuf;
        }
        else
        {
            if (cap > Buffer_cap(buffer))
            {
                if (!__Buffer_reapply(buffer, cap))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

XCL_EXPORT char* XCL_API
Buffer_at(const Buffer* buffer, int pos)
{
    if (pos >= 0 && pos <= buffer->size)
    {
        return buffer->data + pos;
    }
    return NULL;
}
