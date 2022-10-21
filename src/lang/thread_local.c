//
// Created by xuyan on 2022/6/20.
//

#include "lang/thread_local.h"
#include "lang/local_storage.h"
#include <string.h>
#include <xcl/lang/xcl_def.h>
#include <xcl/lang/xcl_err.h>

int32_t
__LocalId_get();

void
__LocalId_recycle(int32_t id);

LocalStorage*
__Thread_getLocalStorage();

bool
__Thread_setLocalStorage(LocalStorage* local_storage);

static LocalStorage*
__ThreadLocal_newLocalStorage()
{
    LocalStorage* local_storage = LocalStorage_new();
    if (!local_storage)
    {
        return NULL;
    }
    memset(local_storage, 0, sizeof(LocalStorage));
    return local_storage;
}

static LocalStorage*
__ThreadLocal_checkoutLocalStorage()
{
    LocalStorage* local_storage = __Thread_getLocalStorage();
    if (!local_storage)
    {
        local_storage = __ThreadLocal_newLocalStorage();
        if (local_storage && !__Thread_setLocalStorage(local_storage))
        {
            LocalStorage_delete(local_storage);
            local_storage = NULL;
        }
    }
    return local_storage;
}

static bool
__ThreadLocal_setData(ThreadLocal* local, const void* src, int len)
{
    if (local->id < 0)
    {
        Err_set(XCL_LOCAL_INVALID_ID);
        return false;
    }
    LocalStorage* local_storage = __ThreadLocal_checkoutLocalStorage();
    if (!local_storage)
    {
        return false;
    }
    return LocalStorage_setTiny(local_storage, local->id, src, len);
}

static void*
__ThreadLocal_getData(ThreadLocal* local)
{
    if (local->id < 0)
    {
        Err_set(XCL_LOCAL_INVALID_ID);
        return NULL;
    }
    LocalStorage* local_storage = __ThreadLocal_checkoutLocalStorage();
    if (!local_storage)
    {
        return NULL;
    }
    return LocalStorage_get(local_storage, local->id);
}

XCL_EXPORT ThreadLocal XCL_API
Local_make()
{
    ThreadLocal result;
    result.id = __LocalId_get();
    return result;
}

XCL_EXPORT void XCL_API
Local_discard(ThreadLocal* local)
{
    if (local->id != -1)
    {
        __LocalId_recycle(local->id);
        local->id = -1;
    }
}

XCL_EXPORT bool XCL_API
Local_set(ThreadLocal* local, void* ptr)
{
    return __ThreadLocal_setData(local, &ptr, sizeof(void*));
}

XCL_EXPORT bool XCL_API
Local_setChar(ThreadLocal* local, char val)
{
    return __ThreadLocal_setData(local, &val, sizeof(char));
}

XCL_EXPORT bool XCL_API
Local_setU8(ThreadLocal* local, unsigned char val)
{
    return __ThreadLocal_setData(local, &val, sizeof(unsigned char));
}

XCL_EXPORT bool XCL_API
Local_setInt32(ThreadLocal* local, int32_t val)
{
    return __ThreadLocal_setData(local, &val, 4);
}

XCL_EXPORT bool XCL_API
Local_setU32(ThreadLocal* local, uint32_t val)
{
    return __ThreadLocal_setData(local, &val, 4);
}

XCL_EXPORT bool XCL_API
Local_setInt16(ThreadLocal* local, int16_t val)
{
    return __ThreadLocal_setData(local, &val, 2);
}

XCL_EXPORT bool XCL_API
Local_setU16(ThreadLocal* local, uint16_t val)
{
    return __ThreadLocal_setData(local, &val, 2);
}

XCL_EXPORT bool XCL_API
Local_setFloat(ThreadLocal* local, float val)
{
    return __ThreadLocal_setData(local, &val, 4);
}

XCL_EXPORT bool XCL_API
Local_setInt64(ThreadLocal* local, int64_t val)
{
    return __ThreadLocal_setData(local, &val, 8);
}

XCL_EXPORT bool XCL_API
Local_setU64(ThreadLocal* local, uint64_t val)
{
    return __ThreadLocal_setData(local, &val, 8);
}

XCL_EXPORT bool XCL_API
Local_setDouble(ThreadLocal* local, double val)
{
    return __ThreadLocal_setData(local, &val, 8);
}

XCL_EXPORT bool XCL_API
Local_get(ThreadLocal* local, void** result)
{
    void* data = __ThreadLocal_getData(local);
    if (!data)
    {
        return false;
    }
    if (result)
    {
        *result = (void*)*(intptr_t*)data;
    }
    return true;
}

XCL_EXPORT bool XCL_API
Local_getChar(ThreadLocal* local, char* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(char*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getU8(ThreadLocal* local, unsigned char* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(unsigned char*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getInt16(ThreadLocal* local, int16_t* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(int16_t*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getU16(ThreadLocal* local, uint16_t* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(uint16_t*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getInt32(ThreadLocal* local, int32_t* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(int32_t*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getU32(ThreadLocal* local, uint32_t* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(uint32_t*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getFloat(ThreadLocal* local, float* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(float*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getInt64(ThreadLocal* local, int64_t* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(int64_t*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getU64(ThreadLocal* local, uint64_t* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(uint64_t*)data;
    }
    return data;
}

XCL_EXPORT bool XCL_API
Local_getDouble(ThreadLocal* local, double* result)
{
    void* data = __ThreadLocal_getData(local);
    if (data && result)
    {
        *result = *(double*)data;
    }
    return data;
}
