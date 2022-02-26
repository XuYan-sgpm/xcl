//
// Created by 徐琰 on 2022/2/8.
//

#include <cstdlib>
#include <xcl/util/concurrent/queue_impl/concurrent_linked_queue_impl.h>

namespace xcl {
ConcurrentLinkedQueueImpl::TaggedNodePtr::TaggedNodePtr(
    ConcurrentLinkedQueueImpl::Node *node, unsigned long tag)
    : TaggedPtr(node, tag) {}
ConcurrentLinkedQueueImpl::Node *
ConcurrentLinkedQueueImpl::TaggedNodePtr::GetNode() const {
  return (ConcurrentLinkedQueueImpl::Node *)ptr();
}
void ConcurrentLinkedQueueImpl::TaggedNodePtr::SetNode(
    ConcurrentLinkedQueueImpl::Node *node) {
  set_ptr(node);
}
ConcurrentLinkedQueueImpl::TaggedNodePtr
ConcurrentLinkedQueueImpl::TaggedNodePtr::GetNextPtr() const {
  return {reinterpret_cast<ConcurrentLinkedQueueImpl::Node *>(ptr()),
          tag() + 1};
}
ConcurrentLinkedQueueImpl::Node *
ConcurrentLinkedQueueImpl::TaggedNodePtr::operator->() const {
  return reinterpret_cast<ConcurrentLinkedQueueImpl::Node *>(ptr());
}
ConcurrentLinkedQueueImpl::TaggedNodePtr
ConcurrentLinkedQueueImpl::Node::GetNext() const {
  return atomic_next_.load(std::memory_order_acquire);
}
void ConcurrentLinkedQueueImpl::Node::SetNext(
    const ConcurrentLinkedQueueImpl::TaggedNodePtr &tagged_node_ptr) {
  atomic_next_.store(tagged_node_ptr, std::memory_order_release);
}
bool ConcurrentLinkedQueueImpl::Node::CasNext(
    bool weak, ConcurrentLinkedQueueImpl::TaggedNodePtr &expect,
    const ConcurrentLinkedQueueImpl::TaggedNodePtr &update) {
  auto m = std::memory_order_acq_rel;
  return weak ? atomic_next_.compare_exchange_weak(expect, update, m)
              : atomic_next_.compare_exchange_strong(expect, update, m);
}
void *ConcurrentLinkedQueueImpl::NodeStack::MemoryPool::Alloc() {
  auto node_size = sizeof(Node);
  if (cursor_.pos) {
    if (cursor_.pos <
        (char *)cursor_.block->address + cursor_.block->block_size) {
      void *p = cursor_.pos;
      cursor_.pos += node_size;
      return p;
    }
  }
  auto rest = total_pool_size_ >> 16;
  auto real_size = 20 * node_size;
  if (rest % node_size == 0) {
    real_size += rest;
  } else {
    real_size += (rest / node_size + 1) * node_size;
  }
  auto block = new Block();
  block->address = new char[real_size];
  block->block_size = real_size;
  block->next_block = nullptr;
  if (head_block_) {
    tail_block_->next_block = block;
  } else {
    head_block_ = tail_block_ = block;
  }
  cursor_.block = block;
  cursor_.pos = (char *)block->address + node_size;
  total_pool_size_ += real_size;
  return block->address;
}
void ConcurrentLinkedQueueImpl::NodeStack::MemoryPool::Release() {
  auto cur_block = head_block_;
  while (cur_block) {
    auto next_block = cur_block->next_block;
    char *memory = reinterpret_cast<char *>(cur_block->address);
    delete[] memory;
    delete cur_block;
    cur_block = next_block;
  }
  head_block_ = tail_block_ = nullptr;
}
ConcurrentLinkedQueueImpl::NodeStack::MemoryPool::~MemoryPool() { Release(); }
bool ConcurrentLinkedQueueImpl::NodeStack::MemoryPool::Available() const {
  return cursor_.pos < reinterpret_cast<char *>(cursor_.block->address) +
                           cursor_.block->block_size;
}
ConcurrentLinkedQueueImpl::Node *ConcurrentLinkedQueueImpl::NodeStack::Get() {
  auto pool = reinterpret_cast<MemoryPool *>(local_mem_pool_.Get());
  if (pool) {
    if (pool->Available()) {
      return reinterpret_cast<Node *>(pool->Alloc());
    }
  } else {
    pool = new MemoryPool();
    local_mem_pool_.Set(pool);
  }
  auto top = atomic_top_.load(std::memory_order_acquire);
  for (;;) {
    if (top) {
      auto top_node = reinterpret_cast<FreeNode *>(top.ptr());
      if (atomic_top_.compare_exchange_weak(top, top.New(top_node->next.ptr()),
                                            std::memory_order_acq_rel)) {
        return reinterpret_cast<Node *>(top_node);
      }
    } else {
      return reinterpret_cast<Node *>(pool->Alloc());
    }
  }
}
void ConcurrentLinkedQueueImpl::NodeStack::Put(
    ConcurrentLinkedQueueImpl::Node *node) {
  auto top = atomic_top_.load(std::memory_order_acquire);
  for (;;) {
    auto new_top = reinterpret_cast<FreeNode *>(node);
    new_top->next.set_ptr(top.ptr());
    if (atomic_top_.compare_exchange_weak(top, {node, top.tag()},
                                          std::memory_order_acq_rel)) {
      return;
    }
  }
}
ConcurrentLinkedQueueImpl::ConcurrentLinkedQueueImpl() {
  auto head_node = reinterpret_cast<Node *>(malloc(sizeof(Node)));
  memset(head_node, 0, sizeof(Node));
  TaggedNodePtr tagged_node_ptr{head_node, 1};
  atomic_head_.store(tagged_node_ptr, std::memory_order_release);
  atomic_tail_.store(tagged_node_ptr, std::memory_order_release);
}
ConcurrentLinkedQueueImpl::~ConcurrentLinkedQueueImpl() {
  //
}
} // namespace xcl
