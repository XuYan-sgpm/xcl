//
// Created by xuyan on 2022/7/19.
//

#include <stdlib.h>
#include <assert.h>
#include "pool/Pool.h"
#include "concurrent/GlobalLock.h"

typedef enum
{
    UNINITIALIZED,
    INITIALIZING,
    INITIALIZED
} CPoolState;

static CPool* __XCL_defaultPool = NULL;

static CPool*
__implInitXclDefPool()
{
    return NULL;
}

XCL_EXPORT CPool* XCL_API
Pool_def()
{
    static volatile CPoolState state = UNINITIALIZED;
    CPool* pool = NULL;
    if (state == INITIALIZED)
    {
        return __XCL_defaultPool;
    }
    __acquireGlobalLock();
    if (state == INITIALIZING)
    {
        assert(false);
    }
    else if (state == UNINITIALIZED)
    {
        state = INITIALIZING;
        __XCL_defaultPool = __implInitXclDefPool();
        pool = __XCL_defaultPool;
        state = INITIALIZED;
    }
    else
    {
        pool = __XCL_defaultPool;
    }
    __releaseGlobalLock();
    return pool;
}

XCL_EXPORT void* XCL_API
Pool_alloc(CPool* pool, uint64_t size)
{
    if (pool && pool->alloc)
    {
        return pool->alloc(pool, size);
    }
    else
    {
        return malloc(size);
    }
}

XCL_EXPORT void XCL_API
Pool_dealloc(CPool* pool, void* ptr, uint64_t size)
{
    if (pool && pool->dealloc)
    {
        pool->dealloc(pool, ptr, size);
    }
    else
    {
        free(ptr);
    }
}

XCL_EXPORT void* XCL_API
Pool_reapply(CPool* pool, void* ptr, uint64_t old, uint64_t req)
{
    if (pool && pool->reapply)
    {
        return pool->reapply(pool, ptr, old, req);
    }
    else
    {
        return realloc(ptr, req);
    }
}
