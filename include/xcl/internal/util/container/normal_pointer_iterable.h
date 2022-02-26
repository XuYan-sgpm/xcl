//
// Created by 徐琰 on 2022/1/12.
//

#ifndef SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_NORMAL_POINTER_ITERABLE_H_
#define SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_NORMAL_POINTER_ITERABLE_H_

#include "xcl/util/container/plain_iterable.h"
namespace xcl {
class NormalPointerIterable : public PlainIterable {
public:
  void *Value() const override;
  bool Next() override;
  bool Equal(const BasePlainIterable &iterable) const override;
  unsigned int operator-(const BasePlainIterable &iterable) const override;
  IterCategory Category() const override;
  void CopyTo(Object *o) override;

  // protected:
  //   void *GetValuePtr() const override;

private:
  void *ptr_;
  const unsigned element_size_;

public:
  NormalPointerIterable(void *p, unsigned element_size);
};
} // namespace xcl

#endif // SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_NORMAL_POINTER_ITERABLE_H_
