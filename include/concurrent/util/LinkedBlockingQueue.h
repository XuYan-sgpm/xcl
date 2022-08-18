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

#include <pool/pool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CLinkedBlockingQueue_st CLinkedBlockingQueue;

#include "lang/xcldef.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief new a linked blocking queue object with default pool
 * @param es element size
 * @param limit queue max element count
 * @return queue object if successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
XCL_EXPORT CLinkedBlockingQueue* XCL_API
LinkedBlockingQueue_new(uint32_t es, uint32_t limit);

/**
 * @brief new a linked blocking queue object with specified pool
 * @param es element size
 * @param limit max queue element count
 * @param pool pool object for node allocation
 * @return queue object
 * @author xuyan
 * @date 2022-08-09
 */
XCL_EXPORT CLinkedBlockingQueue* XCL_API
LinkedBlockingQueue_newByPool(uint32_t es, uint32_t limit, CPool* pool);

/**
 * delete a linked blocking queue object
 * @param queue queue object
 */
XCL_EXPORT void XCL_API
LinkedBlockingQueue_delete(CLinkedBlockingQueue* queue,
                           void (*destructor)(void*, void*),
                           void* usr);

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
                          bool (*constructor)(void*, void*, const void*),
                          void* usr);

/**
 * get first byte data block from queue
 * @param queue queue object
 * @param dst destination memory
 * @param len for input, specified dst capacity;for output, available
 * data size in dst
 * @return true if peek successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
LinkedBlockingQueue_peek(CLinkedBlockingQueue* queue,
                         bool (*constructor)(void*, void*, const void*),
                         void* usr,
                         void* dst);

/**
 * poll first byte data block from queue
 * @param queue queue object
 * @param dst destination memory
 * @param len for input, specified dst capacity;for output, available
 * data size in dst
 * @return true if poll successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
LinkedBlockingQueue_poll(CLinkedBlockingQueue* queue,
                         bool (*move)(void*, void*, void*),
                         void* usr,
                         void* dst);

/**
 * handle if queue is empty
 * @param queue queue object
 * @return true if queue is empty, otherwise false
 */
XCL_EXPORT bool XCL_API
LinkedBlockingQueue_isEmpty(CLinkedBlockingQueue* queue);

/**
 * get queue block count
 * @param queue queue object
 * @return available byte data block count in queue
 */
XCL_EXPORT int32_t XCL_API
LinkedBlockingQueue_size(CLinkedBlockingQueue* queue);

#ifdef __cplusplus
}
#endif
