//
// Created by 徐琰 on 2022/6/25.
//

#include <lang/platform.h>

#if MACOSX

#include <pthread.h>
#include <concurrent/Lock.h>
#include <cstdlib>
#include <sys/time.h>

namespace {
class __InternalMacMutex : public xcl::Lock {
 public:
  ~__InternalMacMutex() override;
  explicit __InternalMacMutex(bool recursive = true);

 public:
  void lock() override;
  void unlock() override;
  bool tryLock() override;

 protected:
  pthread_mutex_t mutex_;
};
void __InternalMacMutex::lock() { ::pthread_mutex_lock(&mutex_); }
void __InternalMacMutex::unlock() { ::pthread_mutex_unlock(&mutex_); }
bool __InternalMacMutex::tryLock() { return ::pthread_mutex_trylock(&mutex_); }
__InternalMacMutex::__InternalMacMutex(bool recursive) : mutex_() {
  if (recursive) {
    pthread_mutexattr_t attr;
    ::pthread_mutexattr_init(&attr);
    ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    ::pthread_mutex_init(&mutex_, &attr);
  } else {
    ::pthread_mutex_init(&mutex_, nullptr);
  }
}
__InternalMacMutex::~__InternalMacMutex() { ::pthread_mutex_destroy(&mutex_); }

class __InternalMacTimedMutex : public xcl::TimedLock,
                                public __InternalMacMutex {
 public:
  bool tryLock(int32_t millis) override;
  void lock() override;
  void unlock() override;
  bool tryLock() override;

 private:
  const static int64_t MAX_DELAY;
};

const int64_t __InternalMacTimedMutex::MAX_DELAY = 1000000L;

bool __InternalMacTimedMutex::tryLock(int32_t millis) {
  const int threshold = 10;
  timespec ts{0, 0};
  timeval st{0, 0};
  gettimeofday(&st, NULL);
  const int64_t kMaxSleep = millis * 1000000L;
  int64_t totalSleep = 0;
  int call = 0;
  while (totalSleep - kMaxSleep < 0) {
    if (::pthread_mutex_trylock(&mutex_) == 0) {
      return true;
    }
    ++call;
    if (call == threshold) {
      timeval et{0, 0};
      gettimeofday(&et, NULL);
      int64_t duration =
          1000000L * (et.tv_sec - st.tv_sec) + (et.tv_usec - st.tv_usec);
      totalSleep = duration * 1000;
      call = 0;
    }
    if (totalSleep - kMaxSleep < 0) {
      ts.tv_nsec = MAX_DELAY;
      if (ts.tv_nsec > kMaxSleep - totalSleep) {
        ts.tv_nsec = kMaxSleep - totalSleep;
      }
      timespec left{0, 0};
      ::nanosleep(&ts, &left);
      totalSleep += ts.tv_nsec - left.tv_nsec;
    }
  }
  return ::pthread_mutex_trylock(&mutex_) == 0;
}
void __InternalMacTimedMutex::lock() { __InternalMacMutex::lock(); }
void __InternalMacTimedMutex::unlock() { __InternalMacMutex::unlock(); }
bool __InternalMacTimedMutex::tryLock() {
  return __InternalMacMutex::tryLock();
}
} // namespace

xcl::Lock *xcl::Lock::NewLock() { return new __InternalMacMutex(); }
xcl::TimedLock *xcl::TimedLock::NewLock() {
  return new __InternalMacTimedMutex();
}

#endif
