//
// Created by 徐琰 on 2022/2/7.
//

#ifndef SCL_SCL_UTIL_OBJECT_POOL_RECYCLER_H_
#define SCL_SCL_UTIL_OBJECT_POOL_RECYCLER_H_

#include "recycled_handle.h"
#include <xcl/util/factory.h>
namespace xcl {
class Recycler : public Object {
private:
  void *const recycle_impl_;

public:
  explicit Recycler(Factory<Object> &factory);
  Recycler(const Recycler &) = delete;
  Recycler(Recycler &&) = delete;
  ~Recycler() override;

public:
  Recycler &operator=(const Recycler &) = delete;

public:
  virtual RecycledHandle *Get();
  virtual bool Recycle(RecycledHandle *handle);
  virtual void Clean();

public:
  Factory<Object> &GetFactory() const;
};
} // namespace xcl

#endif // SCL_SCL_UTIL_OBJECT_POOL_RECYCLER_H_
