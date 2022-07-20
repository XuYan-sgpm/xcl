//
// Created by xuyan on 2022/6/21.
//

#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/XclErr.h"
#include "xcl/pool/CPool.h"
#include <stdlib.h>
#include <string.h>

const static int __initialLocalStorageSize = 8;

static inline int __LocalStorage_grow(CLocalStorage* localStorage, int n)
{
    int32_t cap = localStorage->cap;
    return cap + (cap > n - cap ? cap : n - cap);
}

/*
 * reserve n blocks for passing local storage
 */
static bool __LocalStorage_reserve(CLocalStorage* localStorage, int n)
{
    if (n > localStorage->cap)
    {
        int32_t newCap = __LocalStorage_grow(localStorage, n);
        CStorageBlock* newBlocks = (CStorageBlock*)Pool_reapply(
            NULL,
            localStorage->blocks,
            localStorage->cap * sizeof(CStorageBlock),
            newCap * sizeof(CStorageBlock));
        if (newBlocks)
        {
            localStorage->blocks = newBlocks;
            localStorage->cap = newCap;
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

static CLocalStorage* __LocalStorage_checkMemory(CLocalStorage* localStorage,
                                                 int idx)
{
    if (!localStorage->cap)
    {
        int cap = __initialLocalStorageSize > idx ? __initialLocalStorageSize
                                                  : idx + 1;
        if (!__LocalStorage_reserve(localStorage, cap))
        {
            return NULL;
        }
    }
    else
    {
        if (!__LocalStorage_reserve(localStorage, idx + 1))
        {
            return NULL;
        }
    }
    return localStorage;
}

CLocalStorage* LocalStorage_new()
{
    CLocalStorage* localStorage = Pool_alloc(NULL, sizeof(CLocalStorage));
    if (localStorage)
    {
        memset(localStorage, 0, sizeof(CLocalStorage));
    }
    else
    {
        Err_set(XCL_MEMORY_ERR);
    }
    return localStorage;
}

void* LocalStorage_get(CLocalStorage* localStorage, int idx)
{
    if (!__LocalStorage_checkMemory(localStorage, idx))
    {
        return NULL;
    }
    void* p = localStorage->blocks[idx].data;
    return p;
}

void LocalStorage_delete(CLocalStorage* localStorage)
{
    if (localStorage->blocks)
        Pool_dealloc(NULL,
                     localStorage->blocks,
                     sizeof(CStorageBlock) * localStorage->cap);
    Pool_dealloc(NULL, localStorage, sizeof(CLocalStorage));
}

bool LocalStorage_setPtr(CLocalStorage* localStorage, int idx, intptr_t ptr)
{
    localStorage = __LocalStorage_checkMemory(localStorage, idx);
    if (!localStorage)
    {
        return false;
    }
    void* data = localStorage->blocks[idx].data;
    *(intptr_t*)data = ptr;
    return true;
}

bool LocalStorage_setTiny(CLocalStorage* localStorage,
                          int idx,
                          const void* src,
                          int len)
{
    localStorage = __LocalStorage_checkMemory(localStorage, idx);
    if (!localStorage)
    {
        return false;
    }
    void* data = localStorage->blocks[idx].data;
    memcpy(data, src, len);
    return true;
}
