//
// Created by 徐琰 on 2021/12/24.
//

#ifndef SCL_FIXED_MEMORY_STACK_H
#define SCL_FIXED_MEMORY_STACK_H

#include <xcl/util/allocator/allocator.h>
namespace xcl {
class FixedMemoryStack : public Allocator {
public:
  void *Allocate(unsigned int bytes) override;
  void Deallocate(void *p, unsigned int bytes) override;
  void *Reallocate(void *p, unsigned int old_bytes,
                   unsigned int new_bytes) override;
  ~FixedMemoryStack() override;

private:
  struct Node {
    Node *next;
  };

private:
  Node *top_ = nullptr;
  const unsigned kBlockSize;
  unsigned alloc_count_ = 0;

public:
  explicit FixedMemoryStack(unsigned bs);

public:
  unsigned alloc_count() const;
};
} // namespace xcl

#endif // SCL_FIXED_MEMORY_STACK_H
