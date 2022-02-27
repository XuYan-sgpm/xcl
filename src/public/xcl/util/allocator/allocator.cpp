//
// Created by 徐琰 on 2021/12/5.
//
#include "xcl/util/allocator/allocator.h"

namespace xcl {
unsigned
Allocator::Align(unsigned int bytes, unsigned int a) {
  a = CheckAlignment(a);
  return (bytes + (a - 1)) & (~(a - 1));
}

unsigned
Allocator::CheckAlignment(unsigned int a) {
  if ((a & (a - 1))) { throw; }
  return a;
}
}// namespace xcl
