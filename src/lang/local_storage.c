//
// Created by xuyan on 2022/6/21.
//

#include "lang/local_storage.h"
#include "lang/xcl_err.h"
#include "pool/pool.h"
#include <stdlib.h>
#include <string.h>

const static int __initial_local_storage_size = 8;

static inline int
__LocalStorage_grow(LocalStorage* local_storage, int n)
{
    int32_t cap = local_storage->cap;
    return cap + (cap > n - cap ? cap : n - cap);
}

/*
 * reserve n blocks for passing local storage
 */
static bool
__LocalStorage_reserve(LocalStorage* local_storage, int n)
{
    if (n > local_storage->cap)
    {
        int32_t new_cap = __LocalStorage_grow(local_storage, n);
        StorageBlock* new_blocks = (StorageBlock*)Pool_reallocate(
            Pool_global(),
            local_storage->blocks,
            local_storage->cap * sizeof(StorageBlock),
            new_cap * sizeof(StorageBlock));
        if (new_blocks)
        {
            local_storage->blocks = new_blocks;
            local_storage->cap = new_cap;
            return true;
        }
        Err_set(XCL_MEMORY_ERR);
    }
    else
    {
        return true;
    }
    return false;
}

static LocalStorage*
__LocalStorage_checkMemory(LocalStorage* local_storage, int idx)
{
    if (!local_storage->cap)
    {
        int cap = __initial_local_storage_size > idx
                      ? __initial_local_storage_size
                      : idx + 1;
        if (!__LocalStorage_reserve(local_storage, cap))
        {
            return NULL;
        }
    }
    else
    {
        if (!__LocalStorage_reserve(local_storage, idx + 1))
        {
            return NULL;
        }
    }
    return local_storage;
}

LocalStorage*
LocalStorage_new()
{
    LocalStorage* local_storage =
        Pool_alloc(Pool_global(), sizeof(LocalStorage));
    if (local_storage)
    {
        memset(local_storage, 0, sizeof(LocalStorage));
    }
    else
    {
        Err_set(XCL_MEMORY_ERR);
    }
    return local_storage;
}

void*
LocalStorage_get(LocalStorage* local_storage, int idx)
{
    if (!__LocalStorage_checkMemory(local_storage, idx))
    {
        return NULL;
    }
    void* p = local_storage->blocks[idx].data;
    return p;
}

void
LocalStorage_delete(LocalStorage* local_storage)
{
    if (local_storage->blocks)
        Pool_dealloc(Pool_global(),
                     local_storage->blocks,
                     sizeof(StorageBlock) * local_storage->cap);
    Pool_dealloc(Pool_global(), local_storage, sizeof(LocalStorage));
}

bool
LocalStorage_setPtr(LocalStorage* local_storage, int idx, intptr_t ptr)
{
    local_storage = __LocalStorage_checkMemory(local_storage, idx);
    if (!local_storage)
    {
        return false;
    }
    void* data = local_storage->blocks[idx].data;
    *(intptr_t*)data = ptr;
    return true;
}

bool
LocalStorage_setTiny(LocalStorage* local_storage,
                     int idx,
                     const void* src,
                     int len)
{
    local_storage = __LocalStorage_checkMemory(local_storage, idx);
    if (!local_storage)
    {
        return false;
    }
    void* data = local_storage->blocks[idx].data;
    memcpy(data, src, len);
    return true;
}
