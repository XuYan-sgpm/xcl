//
// Created by 徐琰 on 2022/2/10.
//

#ifndef SCL_SCL_LANG_RUNNABLE_H_
#define SCL_SCL_LANG_RUNNABLE_H_

#include "object.h"
namespace xcl {
class Runnable : public Object {
 public:
  virtual void
  Run() = 0;
};
}// namespace xcl

#endif// SCL_SCL_LANG_RUNNABLE_H_
