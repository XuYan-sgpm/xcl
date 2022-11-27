//
// Created by xuyan on 2022/7/19.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <xcl/lang/xcl_def.h>
#include <stdbool.h>

struct Pool;

typedef struct PoolMethod {
    void* (*const alloc)(struct Pool* pool, uint64_t size);
    void (*const dealloc)(struct Pool* pool, void* ptr, uint64_t size);
    void* (*const reallocate)(struct Pool* pool,
                              void* ptr,
                              uint64_t old,
                              uint64_t req);
} PoolMethod;

typedef struct Pool {
    const struct PoolMethod* const method;
} Pool;

/**
 * @brief init global pool object
 * @return true if success, otherwise false
 */
XCL_EXPORT bool XCL_API
Pool_initGlobal();

/**
 * xcl default pool, may be use jemalloc algorithm
 * available after Pool_initGlobal return true
 * @return default pool object
 */
XCL_EXPORT struct Pool* XCL_API
Pool_global();

/**
 * all functions below can passing NULL as pool object
 * if pool object is NULL, using system default pool
 * to manage memory
 */

/**
 * alloc memory by pool
 * @param pool pool object
 * @param size memory size
 * @return pointer if successfully, otherwise NULL
 */
XCL_EXPORT void* XCL_API
Pool_alloc(struct Pool* pool, uint64_t size);

/**
 * dealloc allocated memory, must be allocated
 * by Pool_alloc
 * @param pool pool object
 * @param ptr pointer to dealloc
 * @param size allocated memory size
 */
XCL_EXPORT void XCL_API
Pool_dealloc(struct Pool* pool, void* ptr, uint64_t size);

/**
 * recycle old memory and alloc new size of memory
 * @param pool pool object
 * @param ptr old allocated pointer
 * @param old allocated memory size
 * @param req required memory size
 * @return pointer if successfully, otherwise NULL
 */
XCL_EXPORT void* XCL_API
Pool_reallocate(struct Pool* pool, void* ptr, uint64_t old, uint64_t req);

#ifdef __cplusplus
}
#endif
