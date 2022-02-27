//
// Created by 徐琰 on 2022/2/8.
//

#include <xcl/util/concurrent/tagged_ptr.h>
namespace xcl {
TaggedPtr::TaggedPtr(void *p, unsigned long tag) : ptr_(p), tag_(tag) {}
void *
TaggedPtr::ptr() const {
  return ptr_;
}
void
TaggedPtr::set_ptr(void *p) {
  ptr_ = p;
}
unsigned long
TaggedPtr::tag() const {
  return tag_;
}
void
TaggedPtr::set_tag(unsigned long tag) {
  tag_ = tag;
}
bool
TaggedPtr::operator==(const TaggedPtr &tagged_ptr) const {
  return ptr_ == tagged_ptr.ptr_ && tag_ == tagged_ptr.tag_;
}
bool
TaggedPtr::operator!=(const TaggedPtr &tagged_ptr) const {
  return !(operator==(tagged_ptr));
}
TaggedPtr::operator bool() const { return ptr_; }
TaggedPtr
TaggedPtr::New(void *p) const {
  return {p, tag_ + 1};
}
TaggedPtr
TaggedPtr::GetNextPtr() const {
  return {ptr_, tag_ + 1};
}
void
TaggedPtr::SetToNext() {
  tag_ += 1;
}
}// namespace xcl
