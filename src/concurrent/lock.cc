//
// Created by 徐琰 on 2022/6/25.
//

#include "concurrent/lock.hpp"

namespace xcl
{
    xcl::Locker::Locker(xcl::Lock* lock) : lock_(lock)
    {
        if (lock_)
        {
            lock_->lock();
        }
    }

    Locker::~Locker()
    {
        if (lock_)
        {
            lock_->unlock();
        }
    }
} // namespace xcl