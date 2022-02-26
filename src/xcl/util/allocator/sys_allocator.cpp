//
// Created by 徐琰 on 2021/12/9.
//

#include <cstdlib>
#include <xcl/util/allocator/sys_allocator.h>

namespace xcl {
__attribute__((destructor)) void releaseSysAllocator() {
  delete &SysAllocator::instance();
}

void *SysAllocator::Allocate(unsigned int bytes) {
  return malloc(Align(bytes, 8));
}

void SysAllocator::Deallocate(void *p, unsigned bytes) { free(p); }

void *SysAllocator::Reallocate(void *p, unsigned int oldBytes,
                               unsigned int newBytes) {
  unsigned newRealBytes = Align(newBytes, 8);
  if (Align(oldBytes, 8) == newRealBytes) {
    return p;
  }
  return realloc(p, newRealBytes);
}

SysAllocator &SysAllocator::instance() {
  if (!globalInstance) {
    globalInstance = new SysAllocator();
  }
  return *globalInstance;
}

SysAllocator *SysAllocator::globalInstance = nullptr;
} // namespace xcl
