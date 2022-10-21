//
// Created by xuyan on 2022/7/21.
//

#include "lang/thread.h"
#include "lang/xcl_err.h"
#include "lang/local_storage.h"
#include "lang/inter_thread_api.h"
#include "pool/pool.h"

void
__Thread_run(void* args)
{
    typedef void (*Proc)(void*);
    Proc run = (Proc)((uintptr_t*)args)[0];
    void* obj = (void*)((uintptr_t*)args)[1];
    run(obj);
    LocalStorage* local_storage = __Thread_getLocalStorage();
    if (local_storage)
    {
        __Thread_setLocalStorage(0);
        LocalStorage_delete(local_storage);
    }
    Pool_dealloc(Pool_def(), args, sizeof(uintptr_t) << 1);
}

XCL_EXPORT Thread XCL_API
Thread_create(void (*proc)(void*), void* obj)
{
    uintptr_t* args = Pool_alloc(Pool_def(), sizeof(uintptr_t) << 1);
    if (!args)
    {
        Err_set(XCL_MEMORY_ERR);
        return (Thread){0};
    }
    args[0] = (uintptr_t)proc;
    args[1] = (uintptr_t)obj;
    uintptr_t handle = __Thread_createHandle(args);
    return (Thread){handle};
}

XCL_EXPORT bool XCL_API
Thread_valid(Thread thread)
{
    return thread.handle;
}

XCL_EXPORT bool XCL_API
Thread_join(Thread* thread)
{
    return Thread_join2(thread, -1);
}

XCL_EXPORT bool XCL_API
Thread_join2(Thread* thread, int32_t timeout)
{
    return __Thread_joinFor(thread->handle, timeout);
}

XCL_EXPORT bool XCL_API
Thread_alive(Thread* thread)
{
    return __Thread_alive(thread->handle);
}

XCL_EXPORT bool XCL_API
Thread_detach(Thread* thread)
{
    return __Thread_detach(thread->handle);
}

XCL_EXPORT Thread XCL_API
Thread_current()
{
    return (Thread){__Thread_currentHandle()};
}

XCL_EXPORT unsigned long XCL_API
Thread_currentId()
{
    return __Thread_currentId();
}

XCL_EXPORT uintptr_t XCL_API
Thread_currentHandle()
{
    return __Thread_currentHandle();
}

XCL_EXPORT void XCL_API
Thread_yield()
{
    __Thread_yield();
}
