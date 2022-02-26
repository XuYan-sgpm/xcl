//
// Created by 徐琰 on 2021/12/5.
//

#ifndef SCL_ALLOCATOR_H
#define SCL_ALLOCATOR_H

namespace xcl {
class Allocator {
public:
  virtual void *Allocate(unsigned bytes) = 0;

  virtual void Deallocate(void *p, unsigned bytes) = 0;

  virtual void *Reallocate(void *p, unsigned old_bytes, unsigned new_bytes) = 0;

protected:
  static unsigned CheckAlignment(unsigned a);

  static unsigned Align(unsigned bytes, unsigned a);

public:
  virtual ~Allocator() = default;
};
} // namespace xcl

#endif // SCL_ALLOCATOR_H
