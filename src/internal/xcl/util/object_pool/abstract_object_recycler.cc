//
// Created by 徐琰 on 2022/1/24.
//

#include <xcl/lang/object.h>
#include <xcl/util/container/simple_object_hash_map.h>
#include <xcl/util/object_pool/abstract_object_recycler.h>
#include <xcl/util/object_pool/abstract_object_recycler_component.h>
#include <xcl/util/object_pool/recycled_handle.h>
namespace xcl {
class NoopHandle : public RecycledHandle {
 public:
  bool
  Recycle() override;
  Object *
  GetObj() const override;
};

bool
NoopHandle::Recycle() {
  return false;
}
Object *
NoopHandle::GetObj() const {
  return nullptr;
}
RecycledHandle *AbstractObjectRecycler::noop_handle_ = new NoopHandle();
std::atomic<int> AbstractObjectRecycler::atomic_id_generator_(-2147483648);
const int AbstractObjectRecycler::kOwnThreadId =
	AbstractObjectRecycler::atomic_id_generator_.fetch_add(1);
AbstractObjectRecycler::AbstractObjectRecycler(
	int max_cap_per_thread, int interval, int max_delayed_queues_per_thread,
	int delayed_queue_interval)
	: kMaxCapPerThread(max_cap_per_thread),
	  kInterval(interval),
	  kMaxDelayedQueuesPerThread(max_delayed_queues_per_thread),
	  kDelayedQueueInterval(delayed_queue_interval) {}
AbstractObjectRecycler::AbstractObjectRecycler()
	: kMaxCapPerThread(4096),
	  kInterval(8),
	  kMaxDelayedQueuesPerThread(4),
	  kDelayedQueueInterval(8) {}
RecycledHandle *
AbstractObjectRecycler::Get() {
  if (kMaxCapPerThread == 0) { return nullptr; }
  auto *stack = reinterpret_cast<HandleStack *>(local_stack_.Get());
  if (!stack) {
	stack = CreateHandleStack();
	local_stack_.Set(stack);
  }
  auto *handle = stack->Pop();
  if (!handle) {
	handle = stack->NewHandle();
	handle->set_object(NewObject(handle));
	if (!handle->GetObj()) {
	  delete handle;
	  return nullptr;
	}
  }
  return handle;
}
HandleStack *
AbstractObjectRecycler::CreateHandleStack() {
  return new HandleStack(this, pthread_self(), kMaxCapPerThread, kInterval,
						 kMaxDelayedQueuesPerThread, kDelayedQueueInterval);
}
bool
AbstractObjectRecycler::Recycle(RecycledHandle *handle) {
  if (handle == noop_handle_) { return false; }
  auto *h = dynamic_cast<DefaultHandle *>(handle);
  if (!h || h->stack()->recycler() != this) { return false; }
  h->RecycleToStack();
  return true;
}
AbstractObjectRecycler::~AbstractObjectRecycler() { ReleaseLocalHandles(); }
void
AbstractObjectRecycler::ReleaseLocalHandles() {
  auto stack = reinterpret_cast<HandleStack *>(local_stack_.Get());
  auto map =
	  reinterpret_cast<SimpleObjectHashMap *>(local_delayed_recycle_map_.Get());
  delete stack;
  delete map;
  local_stack_.Set(nullptr);
  local_delayed_recycle_map_.Set(nullptr);
}
void
HandleStack::Push(DefaultHandle *handle) {
  auto current_thread = pthread_self();
  if (thread_ == current_thread) {
	PushNow(handle);
  } else {
	PushLater(handle, current_thread);
  }
}
void
HandleStack::PushNow(DefaultHandle *handle) {
  if (handle->recycle_id()
	  || !handle->CasLastRecycleId(0, AbstractObjectRecycler::kOwnThreadId))
	return;
  handle->set_recycle_id(AbstractObjectRecycler::kOwnThreadId);
  auto size = handle_size_;
  if (size >= kMaxCap || DropHandle(handle)) { return; }
  if (handle_size_ == handle_cap_) {
	auto new_handle_cap = handle_cap_ << 1;
	if (new_handle_cap > kMaxCap) { new_handle_cap = kMaxCap; }
	auto **new_handles =
		(DefaultHandle **)malloc(sizeof(void *) * new_handle_cap);
	if (!new_handles) { return; }
	memcpy((void *)new_handles, (void *)handles_,
		   handle_size_ * sizeof(void *));
	free(handles_);
	handles_ = new_handles;
	handle_cap_ = new_handle_cap;
  }
  handles_[handle_size_++] = handle;
}
void
HandleStack::PushLater(DefaultHandle *handle, pthread_t thread) {
  if (kMaxDelayedQueues == 0) {
	handle->Release();
	return;
  }
  auto *delayed_recycle_map = reinterpret_cast<SimpleObjectHashMap *>(
	  recycler_->local_delayed_recycle_map_.Get());
  if (!delayed_recycle_map) {
	delayed_recycle_map = new SimpleObjectHashMap();
	recycler_->local_delayed_recycle_map_.Set(delayed_recycle_map);
  }
  auto *queue =
	  reinterpret_cast<HandleOrderQueue *>(delayed_recycle_map->Get(this));
  if (queue == nullptr) {
	if (delayed_recycle_map->Size() >= kMaxDelayedQueues) {
	  delayed_recycle_map->Put(this, HandleOrderQueue::dummy_queue_);
	  return;
	}
	queue = NewOrderQueue(thread);
	if (queue == nullptr) { return; }
	delayed_recycle_map->Put(this, queue);
  } else if (queue == HandleOrderQueue::dummy_queue_) {
	return;
  }
  queue->Add(handle);
}
bool
HandleStack::DropHandle(DefaultHandle *handle) {
  if (!handle->has_recycled()) {
	if (handle_recycle_count_ == kInterval) {
	  handle_recycle_count_ = 0;
	  handle->set_has_recycled(true);
	  return false;
	}
	++handle_recycle_count_;
	handle->Release();
  }
  return true;
}
HandleOrderQueue *
HandleStack::NewOrderQueue(pthread_t t) {
  return HandleOrderQueue::NewOrderQueue(this, t);
}
HandleStack::HandleStack(AbstractObjectRecycler *recycler, pthread_t t,
						 int max_cap, int interval, int max_delayed_queues,
						 int delayed_queue_interval)
	: recycler_(recycler),
	  thread_(t),
	  atomic_available_cap_(max_cap > initial_cap_ ? max_cap : initial_cap_),
	  kMaxDelayedQueues(max_delayed_queues),
	  kMaxCap(max_cap),
	  kInterval(interval),
	  kDelayedQueueInterval(delayed_queue_interval),
	  handle_recycle_count_(interval),
	  cur_queue_(nullptr),
	  prev_queue_(nullptr),
	  atomic_head_queue_(nullptr) {
  handle_cap_ = max_cap > initial_cap_ ? initial_cap_ : max_cap;
  handle_size_ = 0;
  handles_ = new DefaultHandle *[handle_cap_];
}

const unsigned HandleStack::initial_cap_ = 16;

void
HandleStack::SetHeadQueue(HandleOrderQueue *head_queue) {
  std::lock_guard<std::mutex> lock(head_mutex_);
  auto *cur_head_queue = atomic_head_queue_.load(std::memory_order_acquire);
  cur_head_queue->SetNextQueue(head_queue);
  atomic_head_queue_.store(head_queue, std::memory_order_release);
}
unsigned
HandleStack::IncreaseCapacity(unsigned int expected_cap) {
  auto max_cap = this->kMaxCap;
  auto new_cap = handle_cap_;
  do { new_cap <<= 1; } while (new_cap < expected_cap && new_cap < max_cap);
  if (new_cap > max_cap) { new_cap = max_cap; }
  if (new_cap != handle_cap_) {
	auto new_handles = new DefaultHandle *[new_cap];
	memcpy(new_handles, handles_, handle_size_ * sizeof(void *));
	delete[] handles_;
	handles_ = new_handles;
	handle_cap_ = new_cap;
  }
  return new_cap;
}
DefaultHandle *
HandleStack::Pop() {
  auto size = handle_size_;
  if (!size) {
	if (!Scavenge()) { return nullptr; }
	size = handle_size_;
	if (size <= 0) { return nullptr; }
  }
  auto handle = handles_[--size];
  handle_size_ = size;
  if (handle->recycle_id() != handle->GetLastRecycleId()) { return nullptr; }
  handle->set_recycle_id(0);
  handle->SetLastRecycleId(0);
  return handle;
}
DefaultHandle *
HandleStack::NewHandle() {
  return new DefaultHandle(this);
}
bool
HandleStack::Scavenge() {
  if (TryScavenge()) { return true; }
  prev_queue_ = nullptr;
  cur_queue_ = atomic_head_queue_.load(std::memory_order_acquire);
  return false;
}
bool
HandleStack::TryScavenge() {
  HandleOrderQueue *prev = nullptr;
  HandleOrderQueue *cursor = cur_queue_;
  if (cursor == nullptr) {
	cursor = atomic_head_queue_.load(std::memory_order_acquire);
	if (!cursor) { return false; }
  } else {
	prev = prev_queue_;
  }
  bool success = false;
  do {
	if (cursor->Transfer(this)) {
	  success = true;
	  break;
	}
	auto *next = cursor->GetNextQueue();
	auto thread = cursor->thread();
	auto ret = pthread_kill(thread, 0);
	if (ret == ESRCH) {
	  if (cursor->HasFinalData()) {
		for (;;) {
		  if (cursor->Transfer(this)) {
			success = true;
		  } else {
			break;
		  }
		}
	  }
	  if (prev) {
		cursor->ReclaimAllAndUnlink();
		delete cursor;
		prev->SetNextQueue(next);
	  }
	} else {
	  prev = cursor;
	}
	cursor = next;
  } while (!success && cursor);
  prev_queue_ = prev;
  cur_queue_ = cursor;
  return success;
}
AbstractObjectRecycler *
HandleStack::recycler() const {
  return recycler_;
}
HandleStack::~HandleStack() {
  for (auto i = 0; i < handle_size_; i++) { handles_[i]->Release(); }
  delete[] handles_;
  handles_ = nullptr;
  handle_size_ = handle_cap_ = 0;
  auto cur_queue = atomic_head_queue_.load();
  HandleOrderQueue *next_queue;
  while (cur_queue) {
	next_queue = cur_queue->GetNextQueue();
	cur_queue->ReclaimAllAndUnlink();
	delete cur_queue;
	cur_queue = next_queue;
  }
}
void
DefaultHandle::Release() {
  delete obj_;
  obj_ = nullptr;
}
DefaultHandle::DefaultHandle(HandleStack *stack) : stack_(stack) {}
bool
DefaultHandle::Recycle() {
  return stack_->recycler()->Recycle(this);
}
bool
DefaultHandle::CasLastRecycleId(unsigned expect_id, unsigned update_id) {
  return atomic_last_recycle_id_.compare_exchange_strong(
	  expect_id, update_id, std::memory_order_acq_rel);
}
void
DefaultHandle::set_object(Object *o) {
  obj_ = o;
}
unsigned
DefaultHandle::GetLastRecycleId() const {
  return atomic_last_recycle_id_.load(std::memory_order_acquire);
}
unsigned
DefaultHandle::recycle_id() const {
  return recycle_id_;
}
void
DefaultHandle::set_recycle_id(unsigned id) {
  recycle_id_ = id;
}
bool
DefaultHandle::has_recycled() const {
  return has_recycled_;
}
void
DefaultHandle::set_has_recycled(bool v) {
  has_recycled_ = v;
}
HandleStack *
DefaultHandle::stack() const {
  return stack_;
}
Object *
DefaultHandle::GetObj() const {
  return obj_;
}
void
DefaultHandle::set_stack(HandleStack *stack) {
  stack_ = stack;
}
void
DefaultHandle::SetLastRecycleId(unsigned int last_recycle_id) {
  atomic_last_recycle_id_.store(last_recycle_id, std::memory_order_release);
}
void
DefaultHandle::RecycleToStack() {
  if (atomic_last_recycle_id_.load() == recycle_id_ && stack_) {
	stack_->Push(this);
  }
}

HandleOrderQueue::Head::Head(std::atomic<unsigned> *stack_atomic_available_cap)
	: stack_atomic_available_cap_(stack_atomic_available_cap),
	  link_(nullptr) {}
void
HandleOrderQueue::Head::ReclaimAllAndUnlink() {
  auto reclaim_space = 0;
  auto *cur_link = link_;
  while (cur_link) {
	reclaim_space += Link::link_capacity;
	auto *cur_next = cur_link->next;
	auto write_idx = cur_link->GetWriteIdx();
	auto **handles = cur_link->handles;
	for (auto i = cur_link->read_idx; i < write_idx; ++i) {
	  handles[i]->Release();
	}
	delete cur_link;
	cur_link = cur_next;
  }
  stack_atomic_available_cap_->fetch_add(reclaim_space,
										 std::memory_order_acq_rel);
}
void
HandleOrderQueue::Head::Relink(HandleOrderQueue::Link *link) {
  delete link_;
  link_ = link;
  stack_atomic_available_cap_->fetch_add(Link::link_capacity,
										 std::memory_order_acq_rel);
}
HandleOrderQueue::Link *
HandleOrderQueue::Head::NewLink() {
  return ReserveForLink(stack_atomic_available_cap_) ? new Link() : nullptr;
}
bool
HandleOrderQueue::Head::ReserveForLink(
	std::atomic<unsigned> *stack_atomic_available_cap) {
  auto available = stack_atomic_available_cap->load(std::memory_order_acquire);
  for (;;) {
	if (available < Link::link_capacity) { return false; }
	if (stack_atomic_available_cap->compare_exchange_weak(
			available, available - Link::link_capacity,
			std::memory_order_acq_rel)) {
	  return true;
	}
  }
}
HandleOrderQueue::Link *
HandleOrderQueue::Head::link() const {
  return link_;
}
void
HandleOrderQueue::Head::set_link(HandleOrderQueue::Link *link) {
  link_ = link;
}

HandleOrderQueue *const HandleOrderQueue::dummy_queue_ = new HandleOrderQueue();

HandleOrderQueue::HandleOrderQueue()
	: thread_(nullptr),
	  head_(nullptr),
	  tail_link_(nullptr),
	  kId(AbstractObjectRecycler::atomic_id_generator_.fetch_add(1)),
	  kInterval(0),
	  handle_recycle_count_(kInterval) {}
HandleOrderQueue::HandleOrderQueue(HandleStack *stack, pthread_t thread)
	: thread_(thread),
	  head_(&stack->atomic_available_cap_),
	  tail_link_(new Link()),
	  kId(AbstractObjectRecycler::atomic_id_generator_.fetch_add(1)),
	  kInterval(stack->kDelayedQueueInterval),
	  handle_recycle_count_(kInterval) {
  head_.set_link(tail_link_);
}
HandleOrderQueue *
HandleOrderQueue::NewOrderQueue(HandleStack *stack, pthread_t thread) {
  if (Head::ReserveForLink(&stack->atomic_available_cap_)) { return nullptr; }
  auto *queue = new HandleOrderQueue(stack, thread);
  stack->SetHeadQueue(queue);
  return queue;
}
HandleOrderQueue *
HandleOrderQueue::GetNextQueue() const {
  return next_;
}
void
HandleOrderQueue::SetNextQueue(HandleOrderQueue *queue) {
  next_ = queue;
}
void
HandleOrderQueue::ReclaimAllAndUnlink() {
  head_.ReclaimAllAndUnlink();
  next_ = nullptr;
}
void
HandleOrderQueue::Add(DefaultHandle *handle) {
  if (!handle->CasLastRecycleId(0, kId)) return;
  if (handle_recycle_count_ < kInterval) {
	++handle_recycle_count_;
	handle->Release();
	return;
  }
  handle_recycle_count_ = 0;
  Link *tail_link = tail_link_;
  auto write_idx = tail_link->GetWriteIdx();
  if (write_idx == Link::link_capacity) {
	auto *link = head_.NewLink();
	if (!link) { return; }
	tail_link->next = link;
	tail_link_ = tail_link = link;
	write_idx = tail_link->GetWriteIdx();
  }
  tail_link->handles[write_idx] = handle;
  handle->set_stack(nullptr);
  tail_link->SetWriteIdx(write_idx + 1);
}
bool
HandleOrderQueue::HasFinalData() const {
  return tail_link_->read_idx != tail_link_->GetWriteIdx();
}
bool
HandleOrderQueue::Transfer(HandleStack *stack) {
  auto *head_link = head_.link();
  if (!head_link) { return false; }
  if (head_link->read_idx == head_link->GetWriteIdx()) {
	if (!head_link->next) { return false; }
	head_link = head_.link()->next;
	head_.Relink(head_link);
  }
  auto src_start = head_link->read_idx;
  auto src_end = head_link->GetWriteIdx();
  auto src_size = src_end - src_start;
  if (!src_size) { return false; }
  auto dst_size = stack->handle_size_;
  auto expect_size = dst_size + src_size;
  if (expect_size > stack->handle_cap_) {
	auto actual_cap = stack->IncreaseCapacity(expect_size);
	auto stack_rest = actual_cap - dst_size;
	if (src_end > stack_rest + src_start) { src_end = src_start + stack_rest; }
  }
  if (src_start != src_end) {
	auto **src_handles = head_link->handles;
	auto **dst_handles = stack->handles_;
	auto new_stack_size = dst_size;
	for (auto i = src_start; i != src_end; ++i) {
	  auto *handle = src_handles[i];
	  auto handle_recycle_id = handle->recycle_id();
	  auto handle_last_recycle_id = handle->GetLastRecycleId();
	  if (handle_recycle_id == 0) {
		handle->set_recycle_id(handle_last_recycle_id);
	  } else if (handle_recycle_id != handle_last_recycle_id) {
		return false;
	  }
	  src_handles[i] = nullptr;
	  if (stack->DropHandle(handle)) { continue; }
	  handle->set_stack(stack);
	  dst_handles[new_stack_size++] = handle;
	}
	if (src_end == Link::link_capacity && head_link->next) {
	  head_.Relink(head_link->next);
	}
	head_link->read_idx = src_end;
	if (dst_size == new_stack_size) { return false; }
	stack->handle_size_ = new_stack_size;
	return true;
  } else {
	return false;
  }
}
unsigned
HandleOrderQueue::Link::GetWriteIdx() const {
  return atomic_write_idx.load(std::memory_order_acquire);
}
void
HandleOrderQueue::Link::SetWriteIdx(unsigned idx) {
  atomic_write_idx.store(idx, std::memory_order_release);
}
pthread_t
HandleOrderQueue::thread() const {
  return thread_;
}
void
HandleOrderQueue::set_thread(pthread_t thread) {
  thread_ = thread;
}
}// namespace xcl
