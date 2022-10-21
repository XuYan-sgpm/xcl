//
// Created by xuyan on 2022/7/19.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <xcl/lang/xcl_def.h>

typedef struct PoolClass {
    void* (*const alloc)(struct Pool* pool, uint64_t size);
    void (*const dealloc)(struct Pool* pool, void* ptr, uint64_t size);
    void* (*const reapply)(struct Pool* pool,
                           void* ptr,
                           uint64_t old,
                           uint64_t req);
} PoolClass;

typedef struct Pool {
    const struct PoolClass* const clazz;
    // char attach[0];
} Pool;

/**
 * xcl default pool, may be use jemalloc algorithm
 * @return default pool object
 */
XCL_EXPORT struct Pool* XCL_API
Pool_def();

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
Pool_reapply(struct Pool* pool, void* ptr, uint64_t old, uint64_t req);

#ifdef __cplusplus
}
#endif
