//
// Created by xuyan on 2022/7/19.
//

#include <stdlib.h>
#include <assert.h>
#include "pool/pool.h"
#include <stdbool.h>

typedef enum
{
    UNINITIALIZED,
    INITIALIZING,
    INITIALIZED
} CPoolState;

static struct Pool* __XCL_default_pool = NULL;

static struct Pool*
__implInitXclDefPool()
{
    return NULL;
}

XCL_EXPORT struct Pool* XCL_API
Pool_def()
{
    return NULL;
}

static bool
__Pool_available(const struct Pool* pool)
{
    return pool && pool->clazz && pool->clazz->alloc && pool->clazz->dealloc
           && pool->clazz->reapply;
}

XCL_EXPORT void* XCL_API
Pool_alloc(struct Pool* pool, uint64_t size)
{
    if (__Pool_available(pool))
    {
        return pool->clazz->alloc(pool, size);
    }
    else
    {
        return malloc(size);
    }
}

XCL_EXPORT void XCL_API
Pool_dealloc(struct Pool* pool, void* ptr, uint64_t size)
{
    if (!ptr || !size)
        return;
    if (__Pool_available(pool))
    {
        pool->clazz->dealloc(pool, ptr, size);
    }
    else
    {
        free(ptr);
    }
}

XCL_EXPORT void* XCL_API
Pool_reapply(struct Pool* pool, void* ptr, uint64_t old, uint64_t req)
{
    if (__Pool_available(pool))
    {
        return pool->clazz->reapply(pool, ptr, old, req);
    }
    else
    {
        return realloc(ptr, req);
    }
}
