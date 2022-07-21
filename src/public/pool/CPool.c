//
// Created by xuyan on 2022/7/19.
//

#include <stdlib.h>
#include "xcl/pool/CPool.h"

XCL_PUBLIC(CPool*) Pool_def()
{
    return NULL;
}

XCL_PUBLIC(void*) Pool_alloc(CPool* pool, uint64_t size)
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

XCL_PUBLIC(void) Pool_dealloc(CPool* pool, void* ptr, uint64_t size)
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

XCL_PUBLIC(void*)
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
