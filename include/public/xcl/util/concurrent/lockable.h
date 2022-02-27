//
// Created by 徐琰 on 2022/2/18.
//

#ifndef XCL_INCLUDE_XCL_UTIL_CONCURRENT_LOCKABLE_H_
#define XCL_INCLUDE_XCL_UTIL_CONCURRENT_LOCKABLE_H_

namespace xcl {
class Lockable : public Object {
 public:
  virtual void
  Lock() = 0;
  virtual bool
  TryLock() = 0;
  virtual void
  Unlock() = 0;

 public:
  Lockable(const Lockable &) = delete;

 public:
  Lockable &
  operator=(const Lockable &) = delete;
};
}// namespace xcl

#endif// XCL_INCLUDE_XCL_UTIL_CONCURRENT_LOCKABLE_H_
