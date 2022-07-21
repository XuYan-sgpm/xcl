//
// Created by xuyan on 2022/7/21.
//

#include "xcl/lang/CThread.h"
#include "xcl/lang/XclErr.h"
#include "xcl/lang/CThreadLocal.h"
#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CInterThreadApi.h"
#include "xcl/pool/CPool.h"
#include <assert.h>

static CThreadLocal __Thread_localObject = {0};

void __Thread_run(void* args)
{
    Local_set(&__Thread_localObject, (void*)((uintptr_t*)args)[0]);
    typedef void (*Proc)(void*);
    Proc run = (Proc)((uintptr_t*)args)[1];
    void* usr = (void*)((uintptr_t*)args)[2];
    run(usr);
    CLocalStorage* localStorage = __Thread_getLocalStorage();
    if (localStorage)
    {
        __Thread_setLocalStorage(NULL);
        LocalStorage_delete(localStorage);
    }
    Pool_dealloc(Pool_def(), args, sizeof(uintptr_t) << 1);
}

XCL_PUBLIC(bool)
Thread_create(void (*proc)(void*), void* usr, CThread* thread)
{
    uintptr_t* args = Pool_alloc(Pool_def(), sizeof(uintptr_t) * 3);
    if (!args)
    {
        Err_set(XCL_MEMORY_ERR);
        return false;
    }
    args[0] = (uintptr_t)thread;
    args[1] = (uintptr_t)proc;
    args[2] = (uintptr_t)usr;
    return __Thread_createHandle(args, thread);
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
    return __Thread_wait(thread->handle, timeout);
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
    void* ptr;
    if (Local_get(&__Thread_localObject, &ptr))
    {
        return *(CThread*)ptr;
    }
    else
    {
        return (CThread){0};
    }
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

XCL_PUBLIC(void)
Thread_release(CThread* thread)
{
    if (Thread_join(thread))
    {
        __Thread_closeHandle(thread->handle);
        thread->handle = INVALID_THREAD_HANDLE;
    }
}
