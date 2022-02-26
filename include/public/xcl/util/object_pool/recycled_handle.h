//
// Created by 徐琰 on 2021/12/22.
//

#ifndef SCL_HANDLE_H
#define SCL_HANDLE_H

#include <xcl/lang/object.h>
namespace xcl {
class RecycledHandle : public Object {
public:
  virtual bool Recycle() = 0;
  virtual Object *GetObj() const = 0;
};
} // namespace xcl

#endif // SCL_HANDLE_H
