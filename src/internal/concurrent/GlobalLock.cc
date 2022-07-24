//
// Created by 徐琰 on 2022/7/24.
//

#include <cassert>
#include "xcl/concurrent/GlobalLock.h"
#include "xcl/lang/XclDef.h"
#include "xcl/concurrent/CMutex.h"
#include "xcl/pool/CPool.h"

static CMutex* __XCL_globalMutex = nullptr;

extern "C" {
CMutex* __Mutex_newByPool(CPool* pool);

bool __acquireGlobalLock()
{
    return Mutex_lock(__XCL_globalMutex);
}

void __releaseGlobalLock()
{
    Mutex_unlock(__XCL_globalMutex);
}
}

#if CLANG || GNUC
static __attribute__((constructor)) void __initXclGlobalMutex()
{
    __XCL_globalMutex = __Mutex_newByPool(NULL);
    assert(__XCL_globalMutex);
}
#elif defined(ENABLE_CXX)
namespace xcl
{
    namespace
    {
        class GlobalMutexInitializer
        {
        public:
            GlobalMutexInitializer();
        };

        xcl::GlobalMutexInitializer::GlobalMutexInitializer()
        {
            __XCL_globalMutex = __Mutex_newByPool(NULL);
            assert(__XCL_globalMutex);
        }
    }// namespace
    static GlobalMutexInitializer __globalMutexInitializer;
}// namespace xcl
#else
#error "invalid environment"
#endif
