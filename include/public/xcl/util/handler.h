//
// Created by 徐琰 on 2022/2/9.
//

#ifndef SCL_SCL_UTIL_HANDLE_HANDLER_H_
#define SCL_SCL_UTIL_HANDLE_HANDLER_H_

namespace xcl {
class Handler {
 public:
  virtual ~Handler() = default;

 public:
  virtual void
  handle() = 0;
};
}// namespace xcl

#endif// SCL_SCL_UTIL_HANDLE_HANDLER_H_
