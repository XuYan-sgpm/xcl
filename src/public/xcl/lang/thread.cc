//
// Created by 徐琰 on 2022/2/9.
//

#include <mutex>
#include <xcl/lang/platform.h>
#include <xcl/lang/runnable.h>
#include <xcl/lang/thread.h>
#include <xcl/lang/thread_impl.h>
namespace xcl {
Thread Thread::CurrentThread() {
  return thread_impl::ThreadImpl::GetLocalThread();
}
Thread Thread::NewThread(Runnable *runnable) {
  return new thread_impl::ThreadImpl(
      {[](void *arg) { reinterpret_cast<Runnable *>(arg)->Run(); }, runnable});
}
bool Thread::operator!=(const Thread &thread) const {
  return !(operator==(thread));
}
Thread::Thread(void *context) : context_(context) {}
bool Thread::operator==(const Thread &thread) const {
  return *reinterpret_cast<thread_impl::ThreadImpl *>(context_) ==
         *reinterpret_cast<thread_impl::ThreadImpl *>(thread.context_);
}
bool Thread::IsAlive() {
  return reinterpret_cast<thread_impl::ThreadImpl *>(context_)
      ->IsThreadRunning();
}
bool Thread::Join() {
  return reinterpret_cast<thread_impl::ThreadImpl *>(context_)->JoinThread();
}
bool Thread::Detach() {
  return reinterpret_cast<thread_impl::ThreadImpl *>(context_)->DetachThread();
}
bool Thread::AddCallback(Runnable *runnable) {
  return reinterpret_cast<thread_impl::ThreadImpl *>(context_)->PushRunnable(
      {[](void *arg) { reinterpret_cast<Runnable *>(arg)->Run(); }, runnable});
}
Thread::~Thread() {
  if (context_) {
#if MACOSX || LINUX
    auto h = pthread_self();
#else
#endif
    auto thread_impl = reinterpret_cast<thread_impl::ThreadImpl *>(context_);
    if (h != thread_impl->handle()) {
      delete thread_impl;
    }
    context_ = nullptr;
  }
}
Thread::Thread(Thread &&thread) noexcept {
  context_ = thread.context_;
  thread.context_ = nullptr;
}
Thread &Thread::operator=(Thread &&thread) noexcept {
  if (&thread != this) {
    Detach();
    context_ = thread.context_;
    thread.context_ = nullptr;
  }
  return *this;
}
} // namespace xcl
