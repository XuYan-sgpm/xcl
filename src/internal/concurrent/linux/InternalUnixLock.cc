//
// Created by xuyan on 2022/6/25.
//

#include <xcl/concurrent/Lock.h>
#include <pthread.h>

namespace {
class __InternalUnixMutex : public xcl::Lock {
 public:
  explicit __InternalUnixMutex(bool recursive = true);
  ~__InternalUnixMutex() override;

 public:
  void
  lock() override;
  void
  unlock() override;
  bool
  tryLock() override;

 protected:
  pthread_mutex_t mutex_;
};
void
__InternalUnixMutex::lock() {
  ::pthread_mutex_lock(&mutex_);
}
void
__InternalUnixMutex::unlock() {
  ::pthread_mutex_unlock(&mutex_);
}
bool
__InternalUnixMutex::tryLock() {
  return pthread_mutex_trylock(&mutex_) == 0;
}
__InternalUnixMutex::__InternalUnixMutex(bool recursive) : mutex_() {
  if (recursive) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex_, &attr);
  } else {
    pthread_mutex_init(&mutex_, nullptr);
  }
}
__InternalUnixMutex::~__InternalUnixMutex() { pthread_mutex_destroy(&mutex_); }

class __InternalUnixTimedMutex : public xcl::TimedLock,
                                 public __InternalUnixMutex {
 public:
  explicit __InternalUnixTimedMutex(bool recursive = true);

 public:
  bool
  tryLock(int32_t timeout) override;
  void
  lock() override;
  void
  unlock() override;
  bool
  tryLock() override;
};
bool
__InternalUnixTimedMutex::tryLock(int32_t timeout) {
  timespec ts{0, 0};
  clock_gettime(CLOCK_REALTIME, &ts);
  if (timeout > 1000) {
    ts.tv_sec += timeout / 1000;
    timeout = timeout % 1000;
  }
  ts.tv_nsec += (int64_t)timeout * 1000000L;
  if (ts.tv_nsec > 1000000000) {
    ts.tv_sec += ts.tv_nsec / 1000000000;
    ts.tv_nsec = ts.tv_nsec % 1000000000;
  }
  return ::pthread_mutex_timedlock(&mutex_, &ts) == 0;
}
__InternalUnixTimedMutex::__InternalUnixTimedMutex(bool recursive)
    : __InternalUnixMutex(recursive) {}
void
__InternalUnixTimedMutex::lock() {
  __InternalUnixMutex::lock();
}
void
__InternalUnixTimedMutex::unlock() {
  __InternalUnixMutex::unlock();
}
bool
__InternalUnixTimedMutex::tryLock() {
  return __InternalUnixMutex::tryLock();
}
} // namespace

xcl::Lock*
xcl::Lock::NewLock() {
  return new __InternalUnixMutex();
}

xcl::TimedLock*
xcl::TimedLock::NewLock() {
  return new __InternalUnixTimedMutex();
}