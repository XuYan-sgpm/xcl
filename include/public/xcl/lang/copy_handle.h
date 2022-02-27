//
// Created by 徐琰 on 2022/2/26.
//

#ifndef XCL_INCLUDE_PUBLIC_XCL_LANG_COPY_HANDLE_H_
#define XCL_INCLUDE_PUBLIC_XCL_LANG_COPY_HANDLE_H_

#include <xcl/lang/object.h>

#include "element_handle.h"
namespace xcl {
class CopyHandle : public xcl::Object {
 public:
  virtual void *
  Copy(void *dst, void *first, void *last) = 0;
};
}// namespace xcl

#endif//XCL_INCLUDE_PUBLIC_XCL_LANG_COPY_HANDLE_H_
