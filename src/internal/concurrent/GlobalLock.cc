//
// Created by 徐琰 on 2022/7/24.
//

#include <cassert>
#include "xcl/concurrent/GlobalLock.h"
#include "xcl/lang/XclDef.h"
#include "xcl/concurrent/CMutex.h"

static CMutex* __Global_mutex = nullptr;

#ifdef ENABLE_CXX
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
            __Global_mutex = Mutex_new();
            assert(__Global_mutex);
        }
    }// namespace
    static GlobalMutexInitializer __globalMutexInitializer;
}// namespace xcl
#elif CLANG || GNUC
static __attribute__((constructor)) void __initializeGlobalMutex()
{
    __Global_mutex = Mutex_new();
    assert(__Global_mutex);
}
#else
#error "invalid environment"
#endif

extern "C" bool __acquireGlobalLock()
{
    return Mutex_lock(__Global_mutex);
}

extern "C" void __releaseGlobalLock()
{
    Mutex_unlock(__Global_mutex);
}
