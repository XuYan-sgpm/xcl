//
// Created by xuyan on 2022/6/24.
//

#include <lang/platform.h>

#if WINDOWS

#include <synchapi.h>
#include <handleapi.h>
#include <windef.h>
#include <winbase.h>
#include "concurrent/Lock.h"

namespace {
class __InternalMutexLock : public xcl::Lock {
 public:
  ~__InternalMutexLock() override;
  __InternalMutexLock();

 private:
  CRITICAL_SECTION criticalSection_;

 public:
  void lock() override;
  void unlock() override;
  bool tryLock() override;
};
void __InternalMutexLock::lock() { EnterCriticalSection(&criticalSection_); }
void __InternalMutexLock::unlock() { LeaveCriticalSection(&criticalSection_); }
bool __InternalMutexLock::tryLock() {
  return TryEnterCriticalSection(&criticalSection_);
}
__InternalMutexLock::~__InternalMutexLock() = default;
__InternalMutexLock::__InternalMutexLock() {
  InitializeCriticalSection(&criticalSection_);
}

class __InternalTimedMutexLock : public xcl::TimedLock {
 public:
  ~__InternalTimedMutexLock() override;
  __InternalTimedMutexLock();

 public:
  void lock() override;
  void unlock() override;
  bool tryLock() override;
  bool tryLock(int32_t millis) override;

 private:
  HANDLE handle_;
};
void __InternalTimedMutexLock::lock() {
  ::WaitForSingleObject(handle_, INFINITE);
}
void __InternalTimedMutexLock::unlock() { ::ReleaseMutex(handle_); }
bool __InternalTimedMutexLock::tryLock() {
  return ::WaitForSingleObject(handle_, 0) == WAIT_OBJECT_0;
}
bool __InternalTimedMutexLock::tryLock(int32_t millis) {
  return ::WaitForSingleObject(handle_, millis) == WAIT_OBJECT_0;
}
__InternalTimedMutexLock::__InternalTimedMutexLock() : handle_(nullptr) {
  handle_ = ::CreateMutex(nullptr, false, nullptr);
}
__InternalTimedMutexLock::~__InternalTimedMutexLock() {
  if (handle_) {
    ::CloseHandle(handle_);
    handle_ = nullptr;
  }
}
} // namespace

xcl::Lock *xcl::Lock::NewLock() { return new __InternalMutexLock(); }
xcl::Lock *xcl::TimedLock::NewLock() { return new __InternalTimedMutexLock(); }

#endif
