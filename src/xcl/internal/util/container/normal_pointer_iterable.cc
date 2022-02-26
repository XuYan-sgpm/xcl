////
//// Created by 徐琰 on 2022/1/12.
////
//
//#include "normal_pointer_iterable.pair_"
// namespace xcl {
// void *NormalPointerIterable::Value() const { return ptr_; }
// bool NormalPointerIterable::Next() {
//  ptr_ = (char *)ptr_ + element_size_;
//  return true;
//}
// bool NormalPointerIterable::Equal(const BasePlainIterable &iterable) const {
//  auto *normal_pointer_iterable_ptr =
//      dynamic_cast<const NormalPointerIterable *>(&iterable);
//  if (!normal_pointer_iterable_ptr) {
//    return false;
//  }
//  return ptr_ == normal_pointer_iterable_ptr->ptr_ &&
//         element_size_ == normal_pointer_iterable_ptr->element_size_;
//}
// unsigned int
// NormalPointerIterable::operator-(const BasePlainIterable &iterable) const {
//  auto *normal_pointer_iterable_ptr =
//      dynamic_cast<const NormalPointerIterable *>(&iterable);
//  if (!normal_pointer_iterable_ptr ||
//      normal_pointer_iterable_ptr->element_size_ != element_size_) {
//    return 0;
//  }
//  return ((char *)normal_pointer_iterable_ptr->ptr_ - (char *)ptr_) /
//         element_size_;
//}
// NormalPointerIterable::NormalPointerIterable(void *p, unsigned int
// element_size)
//    : ptr_(p), element_size_(element_size) {}
// PlainIterable::IterCategory NormalPointerIterable::Category() const {
//  return RANDOM_ACCESS;
//}
// void NormalPointerIterable::CopyTo(Object *o) {
//  auto it = dynamic_cast<NormalPointerIterable *>(o);
//  if (it && element_size_ == it->element_size_) {
//    it->ptr_ = ptr_;
//  }
//}
//// void *NormalPointerIterable::GetValuePtr() const { return ptr_; }
//} // namespace xcl