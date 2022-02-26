//
// Created by 徐琰 on 2021/12/24.
//

#include <cstdlib>
#include <xcl/internal/util/allocator/fixed_memory_stack.h>

namespace xcl {
void *FixedMemoryStack::Allocate(unsigned int bytes) {
  if (bytes != kBlockSize) {
    throw;
  }
  void *p;
  if (top_) {
    p = top_;
    top_ = top_->next;
  } else {
    p = malloc(bytes);
    ++alloc_count_;
  }
  return p;
}

void FixedMemoryStack::Deallocate(void *p, unsigned int bytes) {
  if (bytes != kBlockSize) {
    throw;
  }
  ((Node *)p)->next = top_;
  top_ = (Node *)p;
}

void *FixedMemoryStack::Reallocate(void *p, unsigned int old_bytes,
                                   unsigned int new_bytes) {
  return nullptr;
}

FixedMemoryStack::FixedMemoryStack(unsigned int bs) : kBlockSize(bs) {}

FixedMemoryStack::~FixedMemoryStack() {
  auto *cur = top_;
  top_ = nullptr;
  while (cur) {
    auto *cur_next = cur->next;
    free(cur);
    --alloc_count_;
    cur = cur_next;
  }
}

unsigned FixedMemoryStack::alloc_count() const { return alloc_count_; }
} // namespace xcl
