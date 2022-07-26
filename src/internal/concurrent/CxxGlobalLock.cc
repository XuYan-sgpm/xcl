//
// Created by xuyan on 2022/7/26.
//

#include <cassert>

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/concurrent/CMutex.h"
#include "xcl/pool/CPool.h"

static CMutex* __XCL_globalMutex = nullptr;

CMutex* __Mutex_newByPool(CPool* pool);

bool __acquireGlobalLock()
{
    return Mutex_lock(__XCL_globalMutex);
}

void __releaseGlobalLock()
{
    Mutex_unlock(__XCL_globalMutex);
}

#ifdef __cplusplus
}
#endif
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
