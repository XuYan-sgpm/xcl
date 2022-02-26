//
// Created by 徐琰 on 2022/2/7.
//

#include <iostream>
#include <xcl/lang/auto_deletable.h>

namespace xcl {
AutoDeletable::AutoDeletable(const AutoDeletable &ref) {
  std::cout << "copy constructor" << std::endl;
  data_ = ref.data_;
  ref_count_ = ref.ref_count_;
  if (ref_count_) {
    ++*ref_count_;
  }
}
AutoDeletable::AutoDeletable(AutoDeletable &&ref) noexcept {
  std::cout << "move constructor" << std::endl;
  data_ = ref.data_;
  ref_count_ = ref.ref_count_;
  ref.data_ = nullptr;
  ref.ref_count_ = nullptr;
}
AutoDeletable::~AutoDeletable() { Delete(); }
AutoDeletable::AutoDeletable(Object *o) {
  data_ = o;
  ref_count_ = new unsigned;
  *ref_count_ = 1;
}
AutoDeletable &AutoDeletable::operator=(const AutoDeletable &ref) {
  std::cout << "copy assigner" << std::endl;
  if (&ref != this) {
    Delete();
    data_ = ref.data_;
    ref_count_ = ref.ref_count_;
    if (ref_count_) {
      ++*ref_count_;
    }
  }
  return *this;
}
AutoDeletable &AutoDeletable::operator=(AutoDeletable &&ref) noexcept {
  std::cout << "move assigner" << std::endl;
  if (&ref != this) {
    Delete();
    data_ = ref.data_;
    ref_count_ = ref.ref_count_;
    ref.data_ = nullptr;
    ref.ref_count_ = nullptr;
  }
  return *this;
}
AutoDeletable &AutoDeletable::operator=(Object *o) {
  if (data_ != o || *ref_count_ != 1) {
    Delete();
    data_ = o;
    ref_count_ = new unsigned;
    *ref_count_ = 1;
  }
  return *this;
}
void AutoDeletable::Delete() {
  if (ref_count_ && --*ref_count_ == 0) {
    delete data_;
    data_ = nullptr;
    ref_count_ = nullptr;
  }
}
bool AutoDeletable::IsValid() const { return ref_count_; }
Object *AutoDeletable::data() const { return data_; }
void AutoDeletable::AddRef(unsigned int delta) {
  if (ref_count_) {
    *ref_count_ += delta;
  }
}
} // namespace xcl
