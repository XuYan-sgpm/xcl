//
// Created by 徐琰 on 2022/2/26.
//

#ifndef XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_BIDIRECTIONAL_ITERATOR_H_
#define XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_BIDIRECTIONAL_ITERATOR_H_

#include "forward_iterator.h"
namespace xcl {
template<typename E>
class BidirectionalIterator : public ForwardIterator<E> {
 public:
  virtual BidirectionalIterator&
  operator--() = 0;
};
}// namespace xcl

#endif//XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_BIDIRECTIONAL_ITERATOR_H_
