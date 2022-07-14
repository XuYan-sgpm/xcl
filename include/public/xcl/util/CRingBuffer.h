//
// Created by xuyan on 2022/7/13.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/XclDef.h>

typedef struct {
    char* const buf;
    int32_t beg, end;
    const int32_t cap;
    const int32_t blockSize;
} CRingBuffer;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * initialize a ring buffer
 * @param ringBuffer ring buffer object
 * @param cap max size of data block
 * @param bs data block size
 * @return true if init successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_init(CRingBuffer* ringBuffer, int32_t cap, int32_t bs);

/**
 * release ring buffer memory
 * @param ringBuffer ring buffer object
 */
XCL_PUBLIC(void)
RingBuffer_release(CRingBuffer* ringBuffer);

/**
 * get ring buffer available block count
 * @param ringBuffer ring buffer object
 * @return buffer current available block count
 */
XCL_PUBLIC(int32_t)
RingBuffer_size(const CRingBuffer* ringBuffer);

/**
 * get ring buffer max block count
 * @param ringBuffer ring buffer object
 * @return max blocks that ring buffer can hold
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
 * push block front into ring buffer
 * @param ringBuffer ring buffer object
 * @param src source block data
 * @param len source block len
 * @return true if push successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_pushFront(CRingBuffer* ringBuffer, const void* src, int32_t len);

/**
 * push block back into ring buffer
 * @param ringBuffer ring buffer object
 * @param src source block data
 * @param len source block len
 * @return true if push successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_pushBack(CRingBuffer* ringBuffer, const void* src, int32_t len);

/**
 * force push data block front into ring buffer, if
 * ring buffer is full, first data block will be overwritten
 * @param ringBuffer ring buffer
 * @param src source data block
 * @param len source block len
 */
XCL_PUBLIC(void)
RingBuffer_forcePushFront(CRingBuffer* ringBuffer, const void* src,
                          int32_t len);

/**
 * force push data block back into ring buffer
 * @param ringBuffer ring buffer object
 * @param src source data block
 * @param len source block len
 */
XCL_PUBLIC(void)
RingBuffer_forcePushBack(CRingBuffer* ringBuffer, const void* src, int32_t len);

/**
 * pop data block front from ring buffer
 * @param ringBuffer ring buffer object
 * @param dst pointer store block data from ring buffer
 * @param len for input, indicates memory size
 * of dst; for output, indicates block size
 * @return true if pop successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_popFront(CRingBuffer* ringBuffer, void* dst, int32_t* len);

/**
 * pop data block back from ring buffer
 * @param ringBuffer ring buffer object
 * @param dst pointer store block data from ring buffer
 * @param len for input, indicates memory size
 * of dst; for output, indicates block size
 * @return true if pop successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_popBack(CRingBuffer* ringBuffer, void* dst, int32_t* len);

/**
 * clear ring buffer
 * @param ringBuffer ring buffer object
 */
XCL_PUBLIC(void)
RingBuffer_clear(CRingBuffer* ringBuffer);

/**
 * get pointer at position idx of ring buffer
 * @param ringBuffer ring buffer object
 * @param idx index
 * @return pointer if idx available, otherwise NULL
 */
XCL_PUBLIC(void*)
RingBuffer_at(const CRingBuffer* ringBuffer, int32_t idx);

/**
 * get data block at position idx of ring buffer
 * @param ringBuffer ring buffer object
 * @param idx position of get
 * @param dst pointer store output block data
 * @param len for input, indicates memory size of dst;
 * for output, indicates block len
 * @return true if get successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_get(const CRingBuffer* ringBuffer, int32_t idx, void* dst,
               int32_t* len);

/**
 * insert a data block at position idx of ring buffer
 * @param ringBuffer ring buffer object
 * @param idx position of insert
 * @param src source data block
 * @param len source block len
 * @return true if insert successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_insert(CRingBuffer* ringBuffer, int32_t idx, const void* src,
                  int32_t len);

/**
 * force insert a data block at position idx of ring buffer
 * note that if ring buffer is full, first block of ring
 * buffer will be overwritten
 * @param ringBuffer ring buffer object
 * @param idx position of insert
 * @param src source data block
 * @param len source block len
 */
XCL_PUBLIC(void)
RingBuffer_forceInsert(CRingBuffer* ringBuffer, int32_t idx, const void* src,
                       int32_t len);

/**
 * repeat insert blocks at position idx of ring buffer
 * @param ringBuffer ring buffer object
 * @param idx position of insert
 * @param count block count
 * @param src source data block
 * @param len source block len
 * @return true if insert successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_insertRepeat(CRingBuffer* ringBuffer, int32_t idx, int32_t count,
                        const void* src, int32_t len);

/**
 * force insert blocks at position idx of ring buffer
 * @param ringBuffer ring buffer object
 * @param idx position of insert
 * @param count block count
 * @param src source data block
 * @param len source block len
 */
XCL_PUBLIC(void)
RingBuffer_forceInsertRepeat(CRingBuffer* ringBuffer, int32_t idx,
                             int32_t count, const void* src, int32_t len);

/**
 * insert continuous blocks at position idx of ring buffer
 * @param ringBuffer ring buffer object
 * @param idx insert position
 * @param src source data block
 * @param bs block size
 * @param count block count
 * @return true if insert successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_insertContinuous(CRingBuffer* ringBuffer, int32_t idx,
                            const void* src, int32_t bs, int32_t count);

/**
 * force insert continuous blocks at position idx of ring buffer
 * if ring buffer is full, blocks from start pos of ring buffer
 * will be overwritten
 * @param ringBuffer ring buffer object
 * @param idx insert position
 * @param src source data block
 * @param bs block size
 * @param count block count
 */
XCL_PUBLIC(void)
RingBuffer_forceInsertContinuous(CRingBuffer* ringBuffer, int32_t idx,
                                 const void* src, int32_t bs, int32_t count);

/**
 * assign data blocks to ring buffer
 * if ring buffer is not empty before invoked
 * all data available in ring buffer will be
 * overwritten
 * @param ringBuffer ring buffer object
 * @param n data block count
 * @param src source data block
 * @param len source block len
 * @return true if assign successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_assignRepeat(CRingBuffer* ringBuffer, int32_t n, const void* src,
                        int32_t len);

/**
 * assign continuous blocks to ring buffer
 * @param ringBuffer ring buffer object
 * @param src pointer store continuous blocks
 * @param bs block size
 * @param count block count
 * @return true if assign successfully, otherwise false
 */
XCL_PUBLIC(bool)
RingBuffer_assignContinuous(CRingBuffer* ringBuffer, const void* src,
                            int32_t bs, int32_t count);

#ifdef __cplusplus
}
#endif
