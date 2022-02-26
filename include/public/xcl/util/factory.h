//
// Created by 徐琰 on 2022/2/18.
//

#ifndef XCL_INCLUDE_XCL_UTIL_FACTORY_H_
#define XCL_INCLUDE_XCL_UTIL_FACTORY_H_

#include <xcl/lang/object.h>
namespace xcl {
template <typename T> class Factory : public Object {
public:
  virtual T *NewElement() const = 0;
};

template <typename T> class DummyFactory : public Factory<T> {
public:
  T *newElement() const override { return nullptr; }
};
} // namespace xcl

#endif // XCL_INCLUDE_XCL_UTIL_FACTORY_H_
