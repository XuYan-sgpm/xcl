/**
 * @author xuyan
 * @brief ring buffer implementation, simple, graceful and powerful
 * only available for pod-like data
 * @version 0.1
 * @date 2022-08-05
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <lang/XclDef.h>

typedef struct {
    char* buf;
    int32_t beg, size;
    int32_t cap;
    int32_t eleSize;
} CRingBuffer;

/**
 * initialize a ring buffer
 * @param ringBuffer ring buffer object
 * @param cap max size of data element
 * @param es data element size
 * @return ring buffer object if successfully, otherwise false
 */
XCL_EXPORT CRingBuffer XCL_API
RingBuffer_make(int32_t cap, int32_t es);

/**
 * release ring buffer memory
 * @param ringBuffer ring buffer object
 */
XCL_EXPORT void XCL_API
RingBuffer_release(CRingBuffer* ringBuffer);

/**
 * get ring buffer available element count
 * @param ringBuffer ring buffer object
 * @return buffer current available element count
 */
XCL_EXPORT int32_t XCL_API
RingBuffer_size(const CRingBuffer* ringBuffer);

/**
 * get ring buffer max element count
 * @param ringBuffer ring buffer object
 * @return max elements that ring buffer can hold
 */
XCL_EXPORT int32_t XCL_API
RingBuffer_capacity(const CRingBuffer* ringBuffer);

/**
 * get if ring buffer is empty
 * @param ringBuffer ring buffer object
 * @return true if ring buffer is empty, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_empty(const CRingBuffer* ringBuffer);

/**
 * push element front into ring buffer
 * @param ringBuffer ring buffer object
 * @param src source element data
 * @param force whether overwrite if ring buffer is full
 * @return true if push successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_pushFront(CRingBuffer* ringBuffer, const void* src, bool force);

/**
 * push element back into ring buffer
 * @param ringBuffer ring buffer object
 * @param src source element data
 * @param force whether overwrite if ring buffer is full
 * @return true if push successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_pushBack(CRingBuffer* ringBuffer, const void* src, bool force);

/**
 * pop data element front from ring buffer
 * @param ringBuffer ring buffer object
 * @param dst pointer store element data from ring buffer
 * @param len for input, indicates memory size
 * of dst; for output, indicates element size
 * @return true if pop successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_popFront(CRingBuffer* ringBuffer, void* dst);

/**
 * pop data element back from ring buffer
 * @param ringBuffer ring buffer object
 * @param dst pointer store element data from ring buffer
 * @param len for input, indicates memory size
 * of dst; for output, indicates element size
 * @return true if pop successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_popBack(CRingBuffer* ringBuffer, void* dst);

/**
 * clear ring buffer
 * @param ringBuffer ring buffer object
 */
XCL_EXPORT void XCL_API
RingBuffer_clear(CRingBuffer* ringBuffer);

/**
 * get pointer at position pos of ring buffer
 * @param ringBuffer ring buffer object
 * @param pos index
 * @return pointer if pos available, otherwise NULL
 */
XCL_EXPORT void* XCL_API
RingBuffer_at(CRingBuffer* ringBuffer, int32_t pos);

/**
 * get data element at position pos of ring buffer
 * @param ringBuffer ring buffer object
 * @param pos position of get
 * @param dst pointer store output element data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_get(const CRingBuffer* ringBuffer, int32_t pos, void* dst);

/**
 * insert a data element at position pos of ring buffer
 * @param ringBuffer ring buffer object
 * @param pos position of insert
 * @param src source data element
 * @param force whether overwrite if ring buffer is full
 * @return true if insert successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_insert(CRingBuffer* ringBuffer,
                  int32_t pos,
                  const void* src,
                  bool force);

/**
 * repeat insert elements at position pos of ring buffer
 * @param ringBuffer ring buffer object
 * @param pos position of insert
 * @param count element count
 * @param src source data element
 * @param force whether overwrite if ring buffer is full
 * @return true if insert successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_insertRepeat(CRingBuffer* ringBuffer,
                        int32_t pos,
                        int32_t count,
                        const void* src,
                        bool force);

/**
 * insert continuous elements at position pos of ring buffer
 * @param ringBuffer ring buffer object
 * @param pos insert position
 * @param src source data element
 * @param es element size
 * @param len element count
 * @param force whether overwrite if ring buffer is full
 * @return true if insert successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_insertRegion(CRingBuffer* ringBuffer,
                        int32_t pos,
                        const void* src,
                        int32_t es,
                        int32_t len,
                        bool force);

/**
 * assign data elements to ring buffer
 * if ring buffer is not empty before invoked
 * all data available in ring buffer will be
 * overwritten
 * @param ringBuffer ring buffer object
 * @param n data element count
 * @param src source data element
 * @return true if assign successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_assignRepeat(CRingBuffer* ringBuffer, int32_t n, const void* src);

/**
 * assign continuous elements to ring buffer
 * @param ringBuffer ring buffer object
 * @param src pointer store continuous elements
 * @param es element size
 * @param len element count
 * @return true if assign successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_assignRegion(CRingBuffer* ringBuffer,
                        const void* src,
                        int32_t es,
                        int32_t len);

/**
 * replace element at position pos of ring buffer with element
 * store at src
 * @param ringBuffer ring buffer
 * @param pos replace position
 * @param src source element
 * @return true if replace successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_replace(CRingBuffer* ringBuffer,
                   int32_t pos,
                   int32_t n,
                   const void* src);

/**
 * replace ring buffer elements at [pos,pos+n)
 * with count source elements stored at src
 * replace will be cancelled if ring buffer is overflow after replace
 * @param ringBuffer ring buffer object
 * @param pos replace position
 * @param n replaced elements count
 * @param count source element count
 * @param src source data element
 * @return true if replace successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_replaceRepeat(CRingBuffer* ringBuffer,
                         int32_t pos,
                         int32_t n,
                         int32_t count,
                         const void* src);

/**
 * replace ring buffer elements at [pos,pos+n)
 * with source data region [src,src+es*len)
 * replace will be cancelled if ring buffer is overflow after replace
 * @param ringBuffer ring buffer object
 * @param pos replace position
 * @param n replaced elements count
 * @param src source element pointer
 * @param es source element size
 * @param len source element count
 * @return true if successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_replaceRegion(CRingBuffer* ringBuffer,
                         int32_t pos,
                         int32_t n,
                         const void* src,
                         int32_t es,
                         int32_t len);

/**
 * assign from source ring buffer
 * @param ringBuffer dest ring buffer
 * @param src source ring buffer
 * @param first source first element position
 * @param last source last element position
 * @return true if assign successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_assignBufRegion(CRingBuffer* ringBuffer,
                           const CRingBuffer* src,
                           int32_t first,
                           int32_t last);

/**
 * assign from source ring buffer
 * @param ringBuffer dest ring buffer
 * @param src source ring buffer
 * @return true if assign successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_assignBuf(CRingBuffer* ringBuffer, const CRingBuffer* src);

/**
 * insert source region [first,last) from ring buffer
 * @param ringBuffer dest ring buffer
 * @param pos insert position
 * @param src source ring buffer
 * @param first first source element position
 * @param last last source element position
 * @param force whether overwrite if ring buffer is overflow
 * @return true if insert successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_insertBufRegion(CRingBuffer* ringBuffer,
                           int32_t pos,
                           const CRingBuffer* src,
                           int32_t first,
                           int32_t last,
                           bool force);

/**
 * replace elements at [pos,pos+n) with source ring buffer
 * region at [first,last)
 * replace will be cancelled if ring buffer is overflow after replace
 * @param ringBuffer dest ring buffer
 * @param pos replace position
 * @param src source ring buffer
 * @param first first source element position
 * @param last last source element position
 * @return true if replace successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
RingBuffer_replaceBufRegion(CRingBuffer* ringBuffer,
                            int32_t pos,
                            int32_t n,
                            const CRingBuffer* src,
                            int32_t first,
                            int32_t last);

/**
 * @brief write ring buffer data at [pos,pos+n) to specified stream
 * @param ringBuffer ring buffer object
 * @param pos write start pos
 * @param n element written
 * @param write write function for data element writing
 * @param usr user specified object for element writing
 * @return total bytes written, not elements
 * @author xuyan
 * @date 2022-08-15
 */
XCL_EXPORT int32_t XCL_API
RingBuffer_writeStream(const CRingBuffer* ringBuffer,
                       int32_t pos,
                       int32_t n,
                       int32_t (*write)(void*, const void*, int32_t),
                       void* usr);

/**
 * @brief write ring buffer data begin at pos to [dst,dst+len) region
 * @param ringBuffer ring buffer object
 * @param pos start pos of write
 * @param dst pointer store data from ring buffer
 * @param len max size of elements can be written
 * @return actually write element count
 * @author xuyan
 * @date 2022-08-16
 */
XCL_EXPORT int32_t XCL_API
RingBuffer_write(const CRingBuffer* ringBuffer,
                 int32_t pos,
                 void* dst,
                 int32_t len);

/**
 * @brief remove elements at [pos,pos+n)
 * @param ringBuffer ring buffer object
 * @param pos start position of remove
 * @param n remove elements count
 * @author xuyan
 * @date 2022-08-16
 */
XCL_EXPORT void XCL_API
RingBuffer_remove(CRingBuffer* ringBuffer, int32_t pos, int32_t n);

#ifdef __cplusplus
}
#endif
