//
// Created by 徐琰 on 2022/2/26.
//

#ifndef XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_RANDOM_ACCESS_ITERATOR_H_
#define XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_RANDOM_ACCESS_ITERATOR_H_

#include <cstddef>

#include "bidirectional_iterator.h"
namespace xcl {
template<typename E>
class RandomAccessIterator : public BidirectionalIterator<E> {
 public:
  virtual RandomAccessIterator&
  operator+=(ptrdiff_t off) = 0;
  virtual RandomAccessIterator&
  operator-=(ptrdiff_t off) = 0;
  virtual ptrdiff_t
  operator-(const RandomAccessIterator& random_access_iterator) const = 0;
};
}// namespace xcl

#endif//XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_RANDOM_ACCESS_ITERATOR_H_
