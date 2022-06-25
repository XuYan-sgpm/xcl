//
// Created by 徐琰 on 2022/6/25.
//

#include <concurrent/Lock.h>

namespace xcl {
xcl::Locker::Locker(xcl::Lock *lock) : lock_(lock) { lock_->lock(); }
Locker::~Locker() { lock_->unlock(); }
} // namespace xcl