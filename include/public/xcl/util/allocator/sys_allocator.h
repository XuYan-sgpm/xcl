//
// Created by 徐琰 on 2021/12/9.
//

#ifndef SCL_SYSALLOCATOR_H
#define SCL_SYSALLOCATOR_H

#include "allocator.h"

namespace xcl {
class SysAllocator final : public Allocator {
 public:
  void *
  Allocate(unsigned int bytes) override;

  void
  Deallocate(void *p, unsigned bytes) override;

  void *
  Reallocate(void *p, unsigned int oldBytes, unsigned int newBytes) override;

 private:
  static SysAllocator *globalInstance;

 private:
  SysAllocator() = default;

 public:
  static SysAllocator &
  instance();

 public:
  SysAllocator(const SysAllocator &) = delete;

  SysAllocator &
  operator=(const SysAllocator &) = delete;
};
}// namespace xcl

#endif// SCL_SYSALLOCATOR_H
