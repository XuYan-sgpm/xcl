//
// Created by 徐琰 on 2022/2/26.
//

#ifndef XCL_INCLUDE_PUBLIC_XCL_LANG_ELEMENT_HANDLE_H_
#define XCL_INCLUDE_PUBLIC_XCL_LANG_ELEMENT_HANDLE_H_

#include "object.h"
namespace xcl {
class ElementHandle : public Object {
 protected:
  class Iterable {
   public:
	virtual void*
	Value() const = 0;
	virtual bool
	Next() = 0;
  };
};
}// namespace xcl

#endif//XCL_INCLUDE_PUBLIC_XCL_LANG_ELEMENT_HANDLE_H_
