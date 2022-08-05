//
// Created by xuyan on 2022/7/5.
//

/**
 * a simplified linked blocking queue based on list
 * using read-write lock separation for optimize
 * simple and graceful
 * cross platform, available under windows, unix, macosx
 * enjoy it!
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CLinkedBlockingQueue_st CLinkedBlockingQueue;

#include "xcl/lang/XclDef.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * new a linked blocking queue object
 * @return queue object if successfully, otherwise NULL
 */
XCL_EXPORT CLinkedBlockingQueue* XCL_API
LinkedBlockingQueue_new(uint32_t blockSize);

/**
 * delete a linked blocking queue object
 * @param queue queue object
 */
XCL_EXPORT void XCL_API
LinkedBlockingQueue_delete(CLinkedBlockingQueue* queue);

/**
 * offer a byte block to queue
 * @param queue queue object
 * @param src data source
 * @param len data length
 * @return true if offer successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
LinkedBlockingQueue_offer(CLinkedBlockingQueue* queue,
                          const void* src,
                          uint32_t len);

/**
 * get first byte data block from queue
 * @param queue queue object
 * @param dst destination memory
 * @param len for input, specified dst capacity;for output, available
 * data size in dst
 * @return true if peek successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
LinkedBlockingQueue_peek(CLinkedBlockingQueue* queue, void* dst, uint32_t* len);

/**
 * poll first byte data block from queue
 * @param queue queue object
 * @param dst destination memory
 * @param len for input, specified dst capacity;for output, available
 * data size in dst
 * @return true if poll successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
LinkedBlockingQueue_poll(CLinkedBlockingQueue* queue, void* dst, uint32_t* len);

/**
 * handle if queue is empty
 * @param queue queue object
 * @return true if queue is empty, otherwise false
 */
XCL_EXPORT bool XCL_API
LinkedBlockingQueue_isEmpty(const CLinkedBlockingQueue* queue);

/**
 * get queue block count
 * @param queue queue object
 * @return available byte data block count in queue
 */
XCL_EXPORT int32_t XCL_API
LinkedBlockingQueue_size(const CLinkedBlockingQueue* queue);

#ifdef __cplusplus
}
#endif
