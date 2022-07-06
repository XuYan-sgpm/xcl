//
// Created by xuyan on 2022/6/24.
//

#define _AMD64_

#include <lang/platform.h>
#include <synchapi.h>
#include <handleapi.h>
#include <windef.h>
#include <winbase.h>
#include "concurrent/Lock.h"

namespace {
class XCL_HIDDEN __InternalWinMutex : public xcl::Lock {
 public:
  ~__InternalWinMutex() override;
  __InternalWinMutex();

 private:
  CRITICAL_SECTION criticalSection_;

 public:
  void lock() override;
  void unlock() override;
  bool tryLock() override;
};
void __InternalWinMutex::lock() { EnterCriticalSection(&criticalSection_); }
void __InternalWinMutex::unlock() { LeaveCriticalSection(&criticalSection_); }
bool __InternalWinMutex::tryLock() {
  return TryEnterCriticalSection(&criticalSection_);
}
__InternalWinMutex::~__InternalWinMutex() {
  DeleteCriticalSection(&criticalSection_);
}
__InternalWinMutex::__InternalWinMutex() {
  InitializeCriticalSection(&criticalSection_);
}

class __InternalWinTimedMutex : public xcl::TimedLock {
 public:
  ~__InternalWinTimedMutex() override;
  __InternalWinTimedMutex();

 public:
  void lock() override;
  void unlock() override;
  bool tryLock() override;
  bool tryLock(int32_t millis) override;

 private:
  HANDLE handle_;
};
void __InternalWinTimedMutex::lock() {
  ::WaitForSingleObject(handle_, INFINITE);
}
void __InternalWinTimedMutex::unlock() { ::ReleaseMutex(handle_); }
bool __InternalWinTimedMutex::tryLock() {
  return ::WaitForSingleObject(handle_, 0) == WAIT_OBJECT_0;
}
bool __InternalWinTimedMutex::tryLock(int32_t millis) {
  return ::WaitForSingleObject(handle_, millis) == WAIT_OBJECT_0;
}
__InternalWinTimedMutex::__InternalWinTimedMutex() : handle_(nullptr) {
  handle_ = ::CreateMutex(nullptr, false, nullptr);
}
__InternalWinTimedMutex::~__InternalWinTimedMutex() {
  if (handle_) {
    ::CloseHandle(handle_);
    handle_ = nullptr;
  }
}
} // namespace

xcl::Lock *xcl::Lock::NewLock() { return new __InternalWinMutex(); }
xcl::TimedLock *xcl::TimedLock::NewLock() {
  return new __InternalWinTimedMutex();
}
