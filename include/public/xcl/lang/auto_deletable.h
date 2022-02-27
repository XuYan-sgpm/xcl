//
// Created by 徐琰 on 2022/2/7.
//

#ifndef SCL_SRC_PUBLIC_SCL_LANG_REFERENCE_H_
#define SCL_SRC_PUBLIC_SCL_LANG_REFERENCE_H_

#include "object.h"
namespace xcl {
class AutoDeletable : public Object {
  Object *data_ = nullptr;
  unsigned *ref_count_ = nullptr;

 public:
  AutoDeletable(const AutoDeletable &ref);
  AutoDeletable(AutoDeletable &&ref) noexcept;
  ~AutoDeletable() override;

 protected:
  explicit AutoDeletable(Object *o);

 public:
  AutoDeletable &
  operator=(const AutoDeletable &ref);
  AutoDeletable &
  operator=(AutoDeletable &&ref) noexcept;
  AutoDeletable &
  operator=(Object *o);

 public:
  void
  Delete();
  bool
  IsValid() const;

 protected:
  Object *
  data() const;
  void
  AddRef(unsigned delta);
};
}// namespace xcl

#endif// SCL_SRC_PUBLIC_SCL_LANG_REFERENCE_H_
