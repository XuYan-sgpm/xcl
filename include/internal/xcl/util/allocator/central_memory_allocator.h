//
// Created by 徐琰 on 2022/2/26.
//
#ifndef XCL_SRC_INTERNAL_XCL_UTIL_ALLOCATOR_CENTRAL_MEMORY_ALLOCATOR_H_
#define XCL_SRC_INTERNAL_XCL_UTIL_ALLOCATOR_CENTRAL_MEMORY_ALLOCATOR_H_

#include <xcl/util/allocator/allocator.h>

#include "size_classes.h"
namespace xcl {
class CentralMemoryAllocator {
 public:
  void*
  Allocate(unsigned size);
  void
  Deallocate(void* p, unsigned size);

 protected:
  void*
  AllocHuge(unsigned size);
  void*
  ThreadCacheAllocSmall(unsigned size);
  void*
  ThreadCacheAllocNormal(unsigned size);

 private:
  static SizeClasses& size_classes_;
  constexpr static unsigned kPageSize = 1 << 13;
  constexpr static unsigned kChunkSize = 1 << 24;
  constexpr static unsigned kPageShifts = 13;
  constexpr static unsigned kChunkShifts = 24;
  constexpr static unsigned kPagesPerChunk = kChunkSize / kPageSize;
  constexpr static unsigned kMaxSmallSize = 28 << 10;

 private:
  CentralMemoryAllocator() = default;

 public:
  CentralMemoryAllocator(const CentralMemoryAllocator&) = delete;

 public:
  CentralMemoryAllocator&
  operator=(const CentralMemoryAllocator&) = delete;
};
}  // namespace xcl
#endif  // XCL_SRC_INTERNAL_XCL_UTIL_ALLOCATOR_CENTRAL_MEMORY_ALLOCATOR_H_
