//
// Created by xuyan on 2022/7/21.
//

#include "xcl/lang/CThread.h"
#include "xcl/lang/XclErr.h"
#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CInterThreadApi.h"
#include "xcl/pool/CPool.h"

void __Thread_run(void* args)
{
    typedef void (*Proc)(void*);
    Proc run = (Proc)((uintptr_t*)args)[0];
    void* usr = (void*)((uintptr_t*)args)[1];
    run(usr);
    CLocalStorage* localStorage = __Thread_getLocalStorage();
    if (localStorage)
    {
        __Thread_setLocalStorage(0);
        LocalStorage_delete(localStorage);
    }
    Pool_dealloc(Pool_def(), args, sizeof(uintptr_t) << 1);
}

XCL_PUBLIC(CThread)
Thread_create(void (*proc)(void*), void* usr)
{
    uintptr_t* args = Pool_alloc(Pool_def(), sizeof(uintptr_t) << 1);
    if (!args)
    {
        Err_set(XCL_MEMORY_ERR);
        return (CThread){0};
    }
    args[0] = (uintptr_t)proc;
    args[1] = (uintptr_t)usr;
    uintptr_t handle = __Thread_createHandle(args);
    return (CThread){handle};
}

bool Thread_valid(CThread thread)
{
    return thread.handle;
}

XCL_PUBLIC(bool)
Thread_join(CThread* thread)
{
    return Thread_join2(thread, -1);
}

XCL_PUBLIC(bool)
Thread_join2(CThread* thread, int32_t timeout)
{
    return __Thread_joinFor(thread->handle, timeout);
}

XCL_PUBLIC(bool)
Thread_alive(CThread* thread)
{
    return __Thread_alive(thread->handle);
}

XCL_PUBLIC(bool)
Thread_detach(CThread* thread)
{
    return __Thread_detach(thread->handle);
}

XCL_PUBLIC(CThread)
Thread_current()
{
    return (CThread){__Thread_currentHandle()};
}

XCL_PUBLIC(unsigned long)
Thread_currentId()
{
    return __Thread_currentId();
}

XCL_PUBLIC(uintptr_t)
Thread_currentHandle()
{
    return __Thread_currentHandle();
}
