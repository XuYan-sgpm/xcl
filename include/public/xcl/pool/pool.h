#include <cstdint>
#include <xcl/lang/XclDef.h>

#pragma once

namespace xcl
{
    class XCL_EXPORT Pool
    {
    public:
        virtual ~Pool() = default;

    public:
        virtual void*
        alloc(uint64_t size)
            = 0;

        virtual void
        dealloc(void* ptr, uint64_t size)
            = 0;

        virtual void*
        realloc(void* ptr, uint64_t oldSize, uint64_t newSize)
            = 0;

    public:
        static Pool*
        sys();
    };
} // namespace xcl
