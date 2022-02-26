//
// Created by 徐琰 on 2022/2/18.
//

#include <xcl/lang/thread_local.h>
#include <xcl/lang/thread_impl.h>
namespace xcl {
void *ThreadLocal::Get() const {
  auto thread_impl = thread_impl::ThreadImpl::GetLocalThread();
  auto storage_ = &thread_impl->local_storage();
  auto local_storage =
      reinterpret_cast<local_storage::LocalStorage *>(storage_);
  return local_storage->GetObject(id_);
}
void ThreadLocal::Set(void *object) {
  auto thread_impl = thread_impl::ThreadImpl::GetLocalThread();
  auto storage_ = &thread_impl->local_storage();
  auto local_storage =
      reinterpret_cast<local_storage::LocalStorage *>(storage_);
  local_storage->PutObject(id_, object);
}
ThreadLocal::ThreadLocal() : id_(local_storage::LocalStorage::GenerateId()) {}
void *ThreadLocal::operator new(size_t size) { return malloc(size); }
void ThreadLocal::operator delete(void *p) { free(p); }
} // namespace xcl
