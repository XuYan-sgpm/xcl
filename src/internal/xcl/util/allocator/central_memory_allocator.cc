//
// Created by 徐琰 on 2022/2/26.
//
#include <xcl/util/allocator/central_memory_allocator.h>
#include <cstdlib>
namespace xcl {
void*
CentralMemoryAllocator::AllocHuge(unsigned int size) {
  return malloc(size);
}
void*
CentralMemoryAllocator::Allocate(unsigned int size) {
  if (size == 0) {
    return nullptr;
  }
  if (size <= kMaxSmallSize) {
    return ThreadCacheAllocSmall(size);
  }
  if (size <= kChunkSize) {
    return ThreadCacheAllocNormal(size);
  }
  auto normalize_size = size_classes_.Normalize(size);
  return AllocHuge(normalize_size);
}
void
CentralMemoryAllocator::Deallocate(void* p, unsigned int size) {}
void*
CentralMemoryAllocator::ThreadCacheAllocSmall(unsigned int size) {
  return nullptr;
}
void*
CentralMemoryAllocator::ThreadCacheAllocNormal(unsigned int size) {
  return nullptr;
}
SizeClasses& CentralMemoryAllocator::size_classes_ = SizeClasses::Instance();
}  // namespace xcl
