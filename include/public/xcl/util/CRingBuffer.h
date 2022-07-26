//
// Created by xuyan on 2022/7/13.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/XclDef.h>

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
XCL_PUBLIC(CRingBuffer)
RingBuffer_make(int32_t cap, int32_t es);

/**
 * release ring buffer memory
 * @param ringBuffer ring buffer object
 */
XCL_PUBLIC(void)
RingBuffer_release(CRingBuffer* ringBuffer);

/**
 * get ring buffer available element count
 * @param ringBuffer ring buffer object
 * @return buffer current available element count
 */
XCL_PUBLIC(int32_t)
RingBuffer_size(const CRingBuffer* ringBuffer);

/**
 * get ring buffer max element count
 * @param ringBuffer ring buffer object
 * @return max elements that ring buffer can hold
 */
XCL_PUBLIC(int32_t)
RingBuffer_capacity(const CRingBuffer* ringBuffer);

/**
 * get if ring buffer is empty
 * @param ringBuffer ring buffer object
 * @return true if ring buffer is empty, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_empty(const CRingBuffer* ringBuffer);

/**
 * push element front into ring buffer
 * @param ringBuffer ring buffer object
 * @param src source element data
 * @param force whether overwrite if ring buffer is full
 * @return true if push successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_pushFront(CRingBuffer* ringBuffer, const void* src, bool force);

/**
 * push element back into ring buffer
 * @param ringBuffer ring buffer object
 * @param src source element data
 * @param force whether overwrite if ring buffer is full
 * @return true if push successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_pushBack(CRingBuffer* ringBuffer, const void* src, bool force);

/**
 * pop data element front from ring buffer
 * @param ringBuffer ring buffer object
 * @param dst pointer store element data from ring buffer
 * @param len for input, indicates memory size
 * of dst; for output, indicates element size
 * @return true if pop successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_popFront(CRingBuffer* ringBuffer, void* dst);

/**
 * pop data element back from ring buffer
 * @param ringBuffer ring buffer object
 * @param dst pointer store element data from ring buffer
 * @param len for input, indicates memory size
 * of dst; for output, indicates element size
 * @return true if pop successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_popBack(CRingBuffer* ringBuffer, void* dst);

/**
 * clear ring buffer
 * @param ringBuffer ring buffer object
 */
XCL_PUBLIC(void)
RingBuffer_clear(CRingBuffer* ringBuffer);

/**
 * get pointer at position pos of ring buffer
 * @param ringBuffer ring buffer object
 * @param pos index
 * @return pointer if pos available, otherwise NULL
 */
XCL_PUBLIC(void*)
RingBuffer_at(CRingBuffer* ringBuffer, int32_t pos);

/**
 * get data element at position pos of ring buffer
 * @param ringBuffer ring buffer object
 * @param pos position of get
 * @param dst pointer store output element data
 * @return true if get successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_get(const CRingBuffer* ringBuffer, int32_t pos, void* dst);

/**
 * insert a data element at position pos of ring buffer
 * @param ringBuffer ring buffer object
 * @param pos position of insert
 * @param src source data element
 * @param force whether overwrite if ring buffer is full
 * @return true if insert successfully, otherwise false
 */
XCL_PUBLIC(bool)
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
XCL_PUBLIC(bool)
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
 * @param count element count
 * @param force whether overwrite if ring buffer is full
 * @return true if insert successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_insertRegion(CRingBuffer* ringBuffer,
                        int32_t pos,
                        const void* src,
                        int32_t es,
                        int32_t count,
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
XCL_PUBLIC(bool)
RingBuffer_assignRepeat(CRingBuffer* ringBuffer, int32_t n, const void* src);

/**
 * assign continuous elements to ring buffer
 * @param ringBuffer ring buffer object
 * @param src pointer store continuous elements
 * @param es element size
 * @param count element count
 * @return true if assign successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_assignRegion(CRingBuffer* ringBuffer,
                        const void* src,
                        int32_t es,
                        int32_t count);

/**
 * replace element at position pos of ring buffer with element
 * store at src
 * @param ringBuffer ring buffer
 * @param pos replace position
 * @param src source element
 * @return true if replace successfully, otherwise false
 */
XCL_PUBLIC(bool)
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
XCL_PUBLIC(bool)
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
XCL_PUBLIC(bool)
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
XCL_PUBLIC(bool)
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
XCL_PUBLIC(bool)
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
XCL_PUBLIC(bool)
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
XCL_PUBLIC(bool)
RingBuffer_replaceBufRegion(CRingBuffer* ringBuffer,
                            int32_t pos,
                            int32_t n,
                            const CRingBuffer* src,
                            int32_t first,
                            int32_t last);

#ifdef __cplusplus
}
#endif
