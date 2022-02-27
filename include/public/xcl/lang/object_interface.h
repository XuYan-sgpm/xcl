//
// Created by 徐琰 on 2022/2/19.
//

#ifndef XCL_INCLUDE_XCL_LANG_OBJECT_INTERFACE_H_
#define XCL_INCLUDE_XCL_LANG_OBJECT_INTERFACE_H_

#include "object.h"
namespace xcl {
class ObjectInterface : public Object {
 public:
  virtual void
  ArgsConstruct(void *dst, unsigned n, ...) = 0;
  virtual void
  ArgsConstruct(void *dst, unsigned n, va_list args) = 0;
  virtual void
  CopyConstruct(void *dst, const void *src) = 0;
  virtual void
  CopyAssign(void *dst, const void *src) = 0;
  virtual void
  MoveConstruct(void *dst, void *src) = 0;
  virtual void
  MoveAssign(void *dst, void *src) = 0;
  virtual void
  Destroy(void *dst) = 0;
};
}// namespace xcl

#endif// XCL_INCLUDE_XCL_LANG_OBJECT_INTERFACE_H_
