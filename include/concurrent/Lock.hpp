//
// Created by xuyan on 2022/6/24.
//

#pragma once

#include "lang/xcldef.h"
#include <cstdint>

namespace xcl
{
    class XCL_EXPORT Lock
    {
    public:
        virtual ~Lock() = default;

        Lock(const Lock&) = delete;

        Lock&
        operator=(const Lock&)
            = delete;

        explicit Lock() = default;

    public:
        /**
         * acquire a lock for current thread
         */
        virtual void
        lock()
            = 0;

        /**
         * release a lock for current thread
         */
        virtual void
        unlock()
            = 0;

        /**
         * try acquire lock for current thread
         * @return true if acquire successfully, otherwise false
         */
        virtual bool
        tryLock()
            = 0;

    public:
        static Lock*
        NewLock();
    };

    class XCL_EXPORT TimedLock : public Lock
    {
    public:
        using Lock::tryLock;

        /**
         * try acquire lock during timeout millis for current thread
         * @param millis timeout
         * @return true if acquire during timeout millis successfully
         * otherwise false
         */
        virtual bool
        tryLock(int32_t millis)
            = 0;

        static TimedLock*
        NewLock();
    };

    class XCL_EXPORT Locker final
    {
    public:
        explicit Locker(Lock* lock);

        ~Locker();

    public:
        Locker&
        operator=(const Locker&)
            = delete;

    private:
        Lock* lock_;
    };
} // namespace xcl