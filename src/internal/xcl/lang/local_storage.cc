//
// Created by 徐琰 on 2022/2/19.
//

#include <cstring>
#include <xcl/lang/local_storage.h>
namespace xcl::local_storage {
std::atomic<unsigned> LocalStorage::atomic_id_{0};
const unsigned LocalStorage::initial_cap_ = 16;
LocalStorage::~LocalStorage() {
  delete[] local_objects_;
  cap_ = 0;
  local_objects_ = nullptr;
}
void *LocalStorage::GetObject(unsigned index) const {
  return index >= cap_ ? nullptr : local_objects_[index];
}
void LocalStorage::PutObject(unsigned int index, void *object) {
  if (index < cap_) {
    local_objects_[index] = object;
    return;
  }
  auto new_cap = GrowStorage(index);
  if (!local_objects_) {
    cap_ = new_cap;
    local_objects_ = new void *[cap_];
  } else {
    auto new_objects = new void *[new_cap];
    memcpy((void *)new_objects, (void *)local_objects_, cap_ * sizeof(void *));
    delete[] local_objects_;
    local_objects_ = new_objects;
    cap_ = new_cap;
  }
  local_objects_[index] = object;
}
unsigned LocalStorage::GenerateId() { return atomic_id_.fetch_add(1); }
unsigned LocalStorage::GrowStorage(unsigned int index) const {
  if (index < cap_) {
    return cap_;
  }
  if (cap_ == 0 && index < initial_cap_)
    return initial_cap_;
  return Grow(cap_ == 0 ? initial_cap_ : cap_, index);
}
unsigned LocalStorage::Grow(unsigned int old, unsigned int expect) {
  auto res = expect - old;
  return old + (old > res ? old : res);
}
} // namespace xcl::local_storage
