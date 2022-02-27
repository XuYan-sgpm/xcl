//
// Created by 徐琰 on 2022/1/10.
//

#ifndef SCL_SCL_UTIL_CONTAINER_PLAIN_ITERABLE_H_
#define SCL_SCL_UTIL_CONTAINER_PLAIN_ITERABLE_H_

#include <xcl/lang/object.h>
namespace xcl {
class BasePlainIterable : public Object {
 public:
  enum IterCategory { INPUT, OUTPUT, FORWARD, BIDIRECTIONAL, RANDOM_ACCESS };

 public:
  virtual bool
  Next() = 0;
  virtual bool
  Equal(const BasePlainIterable &iterable) const = 0;
  virtual unsigned
  operator-(const BasePlainIterable &iterable) const = 0;
  virtual IterCategory
  Category() const = 0;
};

class PlainIterable : public BasePlainIterable {
 public:
  virtual void *
  Value() const = 0;
};

class PlainConstIterable : public BasePlainIterable {
 public:
  virtual const void *
  Value() const = 0;
};
}// namespace xcl

#endif// SCL_SCL_UTIL_CONTAINER_PLAIN_ITERABLE_H_
