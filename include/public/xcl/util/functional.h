//
// Created by 徐琰 on 2022/2/9.
//

#ifndef SCL_SCL_UTIL_FUNCTIONAL_H_
#define SCL_SCL_UTIL_FUNCTIONAL_H_

#include <xcl/lang/object.h>
namespace xcl {
class Functional : public Object {
 public:
  virtual Object *
  Invoke(Object *arg) = 0;
};
}// namespace xcl

#endif// SCL_SCL_UTIL_FUNCTIONAL_H_
