//
// Created by 徐琰 on 2022/2/7.
//

#include <xcl/util/object_pool/recycler.h>
#include <xcl/util/object_pool/abstract_object_recycler.h>
namespace xcl {
class ObjectRecyclerImpl : public AbstractObjectRecycler {
  friend class Recycler;
  Factory<Object> &element_factory_;

protected:
  Object *NewObject(RecycledHandle *handle) override;

public:
  explicit ObjectRecyclerImpl(Factory<Object> &factory);
};
ObjectRecyclerImpl::ObjectRecyclerImpl(Factory<Object> &factory)
    : AbstractObjectRecycler(), element_factory_(factory) {}
Object *ObjectRecyclerImpl::NewObject(RecycledHandle *handle) {
  return handle != AbstractObjectRecycler::noop_handle_
             ? element_factory_.NewElement()
             : nullptr;
}
Recycler::Recycler(Factory<Object> &factory)
    : recycle_impl_(new ObjectRecyclerImpl(factory)) {}
RecycledHandle *Recycler::Get() {
  return reinterpret_cast<ObjectRecyclerImpl *>(recycle_impl_)->Get();
}
bool Recycler::Recycle(RecycledHandle *handle) {
  return reinterpret_cast<ObjectRecyclerImpl *>(recycle_impl_)->Recycle(handle);
}
Factory<Object> &Recycler::GetFactory() const {
  return reinterpret_cast<ObjectRecyclerImpl *>(recycle_impl_)
      ->element_factory_;
}
Recycler::~Recycler() {
  delete reinterpret_cast<ObjectRecyclerImpl *>(recycle_impl_);
}
void Recycler::Clean() {
  reinterpret_cast<ObjectRecyclerImpl *>(recycle_impl_)->ReleaseLocalHandles();
}
} // namespace xcl