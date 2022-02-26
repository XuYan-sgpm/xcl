//
// Created by 徐琰 on 2022/2/19.
//

#ifndef XCL_SRC_XCL_BUILTIN_LANG_THREAD_IMPL_H_
#define XCL_SRC_XCL_BUILTIN_LANG_THREAD_IMPL_H_

#include "local_storage.h"
#include <mutex>
#include <xcl/lang/platform.h>
namespace xcl::thread_impl {
class ThreadImpl final {
#if LINUX || MACOSX
  pthread_t handle_ = nullptr;
  static pthread_key_t local_thread_key_;
#else
#endif
  static bool initialized_;
  const bool is_native_thread_;

  struct CallPair {
    void (*thread_func)(void *) = nullptr;
    void *thread_arg = nullptr;

  public:
    CallPair() = default;
    CallPair(void (*func)(void *), void *arg);
  };
  CallPair thread_running_call_pair_;
  local_storage::LocalStorage local_storage_;

  class ThreadState {
    char state_value_ = 0;

  public:
    explicit ThreadState(char v = 0);

  public:
    void SetStarted();
    bool IsStarted() const;
    void SetError();
    bool IsError() const;
    void SetTerminated();
    bool IsTerminated() const;
    void SetDetached();
    bool IsDetached() const;
    bool IsAlive() const;
    unsigned GetValue() const;
  };
  ThreadState state_;

  mutable std::recursive_mutex state_mutex_;
  using Locker = std::lock_guard<std::recursive_mutex>;

  class CallStack {
    friend class ThreadImpl;
    ThreadImpl *const thread_impl_;
    struct Node {
      CallPair call_pair;
      Node *next = nullptr;
    };
    Node *top_ = nullptr;

  public:
    explicit CallStack(ThreadImpl *thread_impl);
    ~CallStack();

  public:
    void Add(CallPair call_pair);
    void Invoke();
    void Destroy();
  };
  CallStack call_stack_;

protected:
  ThreadImpl(bool is_native_thread);

public:
  explicit ThreadImpl(CallPair call_pair);
  ThreadImpl(const ThreadImpl &) = delete;

public:
  ThreadImpl &operator=(const ThreadImpl &) = delete;
  bool operator==(const ThreadImpl &thread_impl) const;

public:
  bool PushRunnable(CallPair call_pair);
  bool IsThreadRunning();
  bool JoinThread();
  bool DetachThread();
  bool is_native_thread() const;
  pthread_t handle() const;
  local_storage::LocalStorage &local_storage();
  static ThreadImpl *GetLocalThread();
  static pthread_key_t local_thread_key();

private:
  bool InitHandle();
  static void *ThreadRun(void *arg);
  static bool CreateLocalKey();
  static void SetLocalThread(ThreadImpl *thread_impl);
};
} // namespace xcl::thread_impl

#endif // XCL_SRC_XCL_BUILTIN_LANG_THREAD_IMPL_H_
