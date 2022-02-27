//
// Created by 徐琰 on 2022/2/19.
//
#include <xcl/lang/thread_impl.h>
namespace xcl::thread_impl {
ThreadImpl::ThreadState::ThreadState(char v) { state_value_ = v; }
void
ThreadImpl::ThreadState::SetStarted() {
  state_value_ |= 1;
}
bool
ThreadImpl::ThreadState::IsStarted() const {
  return state_value_ & 1;
}
void
ThreadImpl::ThreadState::SetError() {
  state_value_ |= 2;
}
bool
ThreadImpl::ThreadState::IsError() const {
  return state_value_ & 2;
}
void
ThreadImpl::ThreadState::SetTerminated() {
  state_value_ |= 4;
}
bool
ThreadImpl::ThreadState::IsTerminated() const {
  return state_value_ & 4;
}
void
ThreadImpl::ThreadState::SetDetached() {
  state_value_ |= 8;
}
bool
ThreadImpl::ThreadState::IsDetached() const {
  return state_value_ & 8;
}
bool
ThreadImpl::ThreadState::IsAlive() const {
  return state_value_ == 1;
}
unsigned
ThreadImpl::ThreadState::GetValue() const {
  return state_value_;
}
pthread_key_t ThreadImpl::local_thread_key_ = -1;
bool ThreadImpl::initialized_ = CreateLocalKey();
ThreadImpl::CallStack::CallStack(ThreadImpl *thread_impl)
  : thread_impl_(thread_impl) {}
ThreadImpl::CallStack::~CallStack() {
  if (!thread_impl_->is_native_thread()) {
    thread_impl_->JoinThread();
  }
  Destroy();
}
void
ThreadImpl::CallStack::Add(ThreadImpl::CallPair call_pair) {
  auto node = new Node();
  node->call_pair = call_pair;
  node->next = top_;
  top_ = node;
}
void
ThreadImpl::CallStack::Invoke() {
  auto node = top_;
  while (node) {
    node->call_pair.thread_func(node->call_pair.thread_arg);
    node = node->next;
  }
}
void
ThreadImpl::CallStack::Destroy() {
  auto cur_node = top_;
  while (cur_node) {
    auto next_node = cur_node->next;
    delete cur_node;
    cur_node = next_node;
  }
}
ThreadImpl::ThreadImpl(bool is_native_thread)
  : is_native_thread_(is_native_thread), call_stack_(this) {}
void *
ThreadImpl::ThreadRun(void *arg) {
  auto thread_impl = reinterpret_cast<ThreadImpl *>(arg);
  SetLocalThread(thread_impl);
  auto &call_pair = thread_impl->thread_running_call_pair_;
  call_pair.thread_func(call_pair.thread_arg);
  Locker locker(thread_impl->state_mutex_);
  thread_impl->state_.SetTerminated();
  return nullptr;
}
bool
ThreadImpl::InitHandle() {
  if (state_.GetValue() == 0) {
    auto ret = pthread_create(&handle_, nullptr, &ThreadRun, this);
    if (ret != 0) {
      handle_ = nullptr;
      state_.SetError();
    } else {
      state_.SetStarted();
      return true;
    }
  }
  return false;
}
bool
ThreadImpl::PushRunnable(ThreadImpl::CallPair call_pair) {
  Locker locker(state_mutex_);
  if (state_.GetValue() > 1 && !is_native_thread()) {
    return false;
  }
  call_stack_.Add(call_pair);
  return true;
}
bool
ThreadImpl::IsThreadRunning() {
  Locker locker(state_mutex_);
  bool alive = state_.IsAlive();
  if (alive) {
#if MACOSX || LINUX
    auto ret = pthread_kill(handle_, 0);
    if (ret == ESRCH) {
#else
#endif
      state_.SetTerminated();
      alive = false;
    }
  }
  return alive;
}
bool
ThreadImpl::CreateLocalKey() {
#if MACOSX || LINUX
  auto key_ret = pthread_key_create(&local_thread_key_, [](void *arg) {
    auto thread_impl = reinterpret_cast<ThreadImpl *>(arg);
    thread_impl->call_stack_.Invoke();
    if (thread_impl->is_native_thread()) {
      delete thread_impl;
    }
  });
  if (key_ret != 0) {
    local_thread_key_ = -1;
    return false;
  }
  return true;
#else
#endif
}
void
ThreadImpl::SetLocalThread(ThreadImpl *thread_impl) {
  if (initialized_) {
#if MACOSX || LINUX
    pthread_setspecific(ThreadImpl::local_thread_key_, thread_impl);
#else
#endif
  }
}
ThreadImpl *
ThreadImpl::GetLocalThread() {
  if (initialized_) {
#if MACOSX || LINUX
    auto thread_impl = reinterpret_cast<ThreadImpl *>(
        pthread_getspecific(ThreadImpl::local_thread_key_));
    if (!thread_impl) {
      thread_impl = new ThreadImpl(true);
      thread_impl->handle_ = pthread_self();
      thread_impl->state_.SetStarted();
      SetLocalThread(thread_impl);
    }
    return thread_impl;
#else
#endif
  } else {
    return nullptr;
  }
}
ThreadImpl::ThreadImpl(ThreadImpl::CallPair call_pair) : ThreadImpl(false) {
  thread_running_call_pair_ = call_pair;
  InitHandle();
}
bool
ThreadImpl::JoinThread() {
  Locker locker(state_mutex_);
  if (state_.IsAlive()) {
    state_mutex_.unlock();
    bool success;
#if MACOSX || LINUX
    auto ret = pthread_join(handle_, nullptr);
    success = ret == 0;
#else
#endif
    state_mutex_.lock();
    if (success)
      state_.SetTerminated();
    return success;
  } else {
    return false;
  }
}
bool
ThreadImpl::DetachThread() {
  Locker locker(state_mutex_);
  if (state_.IsAlive()) {
#if MACOSX || LINUX
    pthread_detach(handle_);
#else
#endif
    state_.SetDetached();
    return true;
  } else {
    return false;
  }
}
pthread_key_t
ThreadImpl::local_thread_key() {
  return local_thread_key_;
}
bool
ThreadImpl::operator==(const ThreadImpl &thread_impl) const {
  return handle_ == thread_impl.handle_;
}
bool
ThreadImpl::is_native_thread() const {
  return is_native_thread_;
}
pthread_t
ThreadImpl::handle() const {
  return handle_;
}
local_storage::LocalStorage &
ThreadImpl::local_storage() {
  return local_storage_;
}
__attribute__((destructor)) void
DeleteLocalThreadKey() {
#if MACOSX || LINUX
  if (ThreadImpl::local_thread_key() != -1)
    pthread_key_delete(ThreadImpl::local_thread_key());
#else
#endif
}
ThreadImpl::CallPair::CallPair(void (*func)(void *), void *arg) {
  thread_func = func;
  thread_arg = arg;
}
}  // namespace xcl::thread_impl
