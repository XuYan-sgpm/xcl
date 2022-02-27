//
// Created by 徐琰 on 2022/1/13.
//

#ifndef SCL_SCL_LANG_COPYABLE_H_
#define SCL_SCL_LANG_COPYABLE_H_

#include "object.h"
namespace xcl {
class Copyable : public Object {
 public:
  virtual void
  CopyTo(Object *o) = 0;
};
}// namespace xcl

#endif// SCL_SCL_LANG_COPYABLE_H_
