//
// Created by xuyan on 2022/7/19.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <xcl/lang/XclDef.h>

typedef struct _CPool_st {
    void* (*const alloc)(struct _CPool_st* pool, uint64_t size);
    void (*const dealloc)(struct _CPool_st* pool, void* ptr, uint64_t size);
    void* (*const reapply)(struct _CPool_st* pool, void* ptr, uint64_t old,
                           uint64_t req);
    char attach[0];
} CPool;

/**
 * using malloc, free and realloc for default pool invoke
 * @return default pool object
 */
XCL_PUBLIC(CPool*) Pool_def();

/**
 * all functions below can passing NULL as pool object
 * if pool object is NULL, using xcl default pool
 * to manage memory
 */

/**
 * alloc memory by CPool
 * @param pool pool object
 * @param size memory size
 * @return pointer if successfully, otherwise NULL
 */
XCL_PUBLIC(void*) Pool_alloc(CPool* pool, uint64_t size);

/**
 * dealloc allocated memory, must be allocated
 * by Pool_alloc
 * @param pool pool object
 * @param ptr pointer to dealloc
 * @param size allocated memory size
 */
XCL_PUBLIC(void) Pool_dealloc(CPool* pool, void* ptr, uint64_t size);

/**
 * recycle old memory and alloc new size of memory
 * @param pool pool object
 * @param ptr old allocated pointer
 * @param old allocated memory size
 * @param req required memory size
 * @return pointer if successfully, otherwise NULL
 */
XCL_PUBLIC(void*)
Pool_reapply(CPool* pool, void* ptr, uint64_t old, uint64_t req);

#ifdef __cplusplus
}
#endif
