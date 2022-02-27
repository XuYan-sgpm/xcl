//
// Created by 徐琰 on 2022/1/24.
//

#ifndef SCL_SRC_BUILTIN_SCL_UTIL_OBJECT_POOL_ABSTRACT_OBJECT_RECYCLER_H_
#define SCL_SRC_BUILTIN_SCL_UTIL_OBJECT_POOL_ABSTRACT_OBJECT_RECYCLER_H_

#include <xcl/lang/thread_local.h>
#include <xcl/util/object_pool/recycled_handle.h>

#include <atomic>

namespace xcl {
class HandleStack;
class AbstractObjectRecycler {
  friend class HandleStack;

 public:
  static RecycledHandle *noop_handle_;
  static std::atomic<int> atomic_id_generator_;
  const static int kOwnThreadId;

 private:
  const int kMaxCapPerThread;
  const int kInterval;
  const int kMaxDelayedQueuesPerThread;
  const int kDelayedQueueInterval;

 public:
  RecycledHandle *
  Get();
  bool
  Recycle(RecycledHandle *handle);
  void
  ReleaseLocalHandles();

 protected:
  virtual Object *
  NewObject(RecycledHandle *handle) = 0;

 public:
  AbstractObjectRecycler(int max_cap_per_thread, int interval,
						 int max_delayed_queues_per_thread,
						 int delayed_queue_interval);

 private:
  ThreadLocal local_stack_;
  ThreadLocal local_delayed_recycle_map_;

 private:
  HandleStack *
  CreateHandleStack();

 public:
  AbstractObjectRecycler();
  virtual ~AbstractObjectRecycler();
};
}// namespace xcl

#endif// SCL_SRC_BUILTIN_SCL_UTIL_OBJECT_POOL_ABSTRACT_OBJECT_RECYCLER_H_
