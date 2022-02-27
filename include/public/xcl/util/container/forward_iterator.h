//
// Created by 徐琰 on 2022/2/26.
//

#ifndef XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_FORWARD_ITERATOR_H_
#define XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_FORWARD_ITERATOR_H_

#include "iterator.h"
namespace xcl {
template<typename E>
class ForwardIterator : public Iterator<E> {};
}// namespace xcl

#endif//XCL_INCLUDE_PUBLIC_XCL_UTIL_CONTAINER_FORWARD_ITERATOR_H_
