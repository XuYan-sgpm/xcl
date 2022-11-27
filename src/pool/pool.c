//
// Created by xuyan on 2022/7/19.
//

#include <stdlib.h>
#include <assert.h>
#include "pool/pool.h"
#include <stdbool.h>

static struct Pool* __xcl_global_pool = NULL;

bool XCL_API
Pool_initGlobal()
{
    return false;
}

struct Pool* XCL_API
Pool_global()
{
    return NULL;
}

void* XCL_API
Pool_alloc(struct Pool* pool, uint64_t size)
{
    if (pool && pool->method->alloc)
    {
        return pool->method->alloc(pool, size);
    }
    else
    {
        return malloc(size);
    }
}

void XCL_API
Pool_dealloc(struct Pool* pool, void* ptr, uint64_t size)
{
    if (!ptr || !size)
        return;
    if (pool && pool->method->dealloc)
    {
        pool->method->dealloc(pool, ptr, size);
    }
    else
    {
        free(ptr);
    }
}

void* XCL_API
Pool_reallocate(struct Pool* pool, void* ptr, uint64_t old, uint64_t req)
{
    if (pool && pool->method->reallocate)
    {
        return pool->method->reallocate(pool, ptr, old, req);
    }
    else
    {
        return realloc(ptr, req);
    }
}
