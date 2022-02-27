//
// Created by 徐琰 on 2022/1/15.
//

#ifndef SCL_SRC_BUILTIN_SCL_UTIL_OBJECT_POOL_ABSTRACT_OBJECT_RECYCLER_COMPONENT_H_
#define SCL_SRC_BUILTIN_SCL_UTIL_OBJECT_POOL_ABSTRACT_OBJECT_RECYCLER_COMPONENT_H_

#include <xcl/util/object_pool/recycled_handle.h>

#include <mutex>

namespace xcl {
class HandleOrderQueue;
class DefaultHandle;
class HandleStack;
class AbstractObjectRecycler;

class DefaultHandle : public RecycledHandle {
  std::atomic<unsigned> atomic_last_recycle_id_{0};
  unsigned recycle_id_ = 0;
  bool has_recycled_ = false;
  Object *obj_ = nullptr;
  HandleStack *stack_;

 public:
  explicit DefaultHandle(HandleStack *stack);

 public:
  bool
  Recycle() override;
  bool
  CasLastRecycleId(unsigned expect_id, unsigned update_id);
  void
  SetLastRecycleId(unsigned last_recycle_id);
  void
  set_object(Object *o);
  unsigned
  GetLastRecycleId() const;
  unsigned
  recycle_id() const;
  void
  set_recycle_id(unsigned id);
  bool
  has_recycled() const;
  void
  set_has_recycled(bool v);
  HandleStack *
  stack() const;
  void
  set_stack(HandleStack *stack);
  Object *
  GetObj() const override;
  void
  RecycleToStack();
  void
  Release();
};

class HandleOrderQueue final {
  pthread_t thread_;

 public:
  static HandleOrderQueue *const dummy_queue_;

 public:
  struct Link {
	constexpr static unsigned link_capacity = 16;
	std::atomic<unsigned> atomic_write_idx{0};
	DefaultHandle *handles[link_capacity];
	unsigned read_idx = 0;
	Link *next = nullptr;

	unsigned
	GetWriteIdx() const;
	void
	SetWriteIdx(unsigned idx);
  };

  class Head {
	std::atomic<unsigned> *stack_atomic_available_cap_;
	Link *link_;

   public:
	Head(std::atomic<unsigned> *stack_atomic_available_cap);

   public:
	void
	ReclaimAllAndUnlink();
	void
	Relink(Link *link);
	Link *
	NewLink();
	static bool
	ReserveForLink(std::atomic<unsigned> *stack_atomic_available_cap);
	Link *
	link() const;
	void
	set_link(Link *link);
  };

 private:
  Head head_;
  Link *tail_link_;
  HandleOrderQueue *next_ = nullptr;
  const unsigned kId;
  const unsigned kInterval;
  unsigned handle_recycle_count_;

 private:
  HandleOrderQueue();
  HandleOrderQueue(HandleStack *stack, pthread_t thread);

 public:
  static HandleOrderQueue *
  NewOrderQueue(HandleStack *stack, pthread_t thread);
  HandleOrderQueue *
  GetNextQueue() const;
  void
  SetNextQueue(HandleOrderQueue *queue);
  void
  ReclaimAllAndUnlink();
  void
  Add(DefaultHandle *handle);
  bool
  HasFinalData() const;
  bool
  Transfer(HandleStack *stack);
  pthread_t
  thread() const;
  void
  set_thread(pthread_t thread);
};

class HandleStack final {
  friend class HandleOrderQueue;

 private:
  AbstractObjectRecycler *const recycler_;
  const pthread_t thread_;
  std::atomic<unsigned> atomic_available_cap_;
  const unsigned kMaxDelayedQueues;
  const unsigned kMaxCap;
  const unsigned kInterval;
  const unsigned kDelayedQueueInterval;
  unsigned handle_recycle_count_;
  HandleOrderQueue *cur_queue_, *prev_queue_;
  std::atomic<HandleOrderQueue *> atomic_head_queue_;
  std::mutex head_mutex_;

 public:
  unsigned handle_size_;
  unsigned handle_cap_;
  DefaultHandle **handles_;
  const static unsigned initial_cap_;

 public:
  HandleStack(AbstractObjectRecycler *recycler, pthread_t t, int max_cap,
			  int interval, int max_delayed_queues, int delayed_queue_interval);
  ~HandleStack();

 public:
  void
  SetHeadQueue(HandleOrderQueue *head_queue);
  unsigned
  IncreaseCapacity(unsigned expected_cap);
  DefaultHandle *
  Pop();
  void
  Push(DefaultHandle *handle);
  bool
  DropHandle(DefaultHandle *handle);
  DefaultHandle *
  NewHandle();
  AbstractObjectRecycler *
  recycler() const;

 private:
  bool
  Scavenge();
  bool
  TryScavenge();
  void
  PushNow(DefaultHandle *handle);
  void
  PushLater(DefaultHandle *handle, pthread_t thread);
  HandleOrderQueue *
  NewOrderQueue(pthread_t t);
};
}// namespace xcl

#endif// SCL_SRC_BUILTIN_SCL_UTIL_OBJECT_POOL_ABSTRACT_OBJECT_RECYCLER_COMPONENT_H_
