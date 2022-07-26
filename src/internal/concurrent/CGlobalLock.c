//
// Created by 徐琰 on 2022/7/24.
//

#ifndef ENABLE_CXX
#include <assert.h>
#include "xcl/concurrent/GlobalLock.h"
#include "xcl/lang/XclDef.h"
#include "xcl/concurrent/CMutex.h"
#include "xcl/pool/CPool.h"

static CMutex* __XCL_globalMutex = NULL;

CMutex* __Mutex_newByPool(CPool* pool);

bool __acquireGlobalLock()
{
    return Mutex_lock(__XCL_globalMutex);
}

void __releaseGlobalLock()
{
    Mutex_unlock(__XCL_globalMutex);
}

#if CLANG || GNUC
static __attribute__((constructor)) void __initXclGlobalMutex()
{
    __XCL_globalMutex = __Mutex_newByPool(NULL);
    assert(__XCL_globalMutex);
}
#else
#error "global lock can not be initialized"
#endif
#endif
