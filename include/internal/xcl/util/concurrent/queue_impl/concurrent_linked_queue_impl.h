//
// Created by 徐琰 on 2022/2/8.
//

#ifndef SCL_SRC_PUBLIC_SCL_UTIL_CONCURRENT_QUEUES_IMPL_CONCURRENT_LINKED_QUEUE_IMPL_H_
#define SCL_SRC_PUBLIC_SCL_UTIL_CONCURRENT_QUEUES_IMPL_CONCURRENT_LINKED_QUEUE_IMPL_H_

#include "xcl/util/concurrent/tagged_ptr.h"
#include <atomic>
#include <xcl/lang/object.h>
#include <xcl/lang/thread_local.h>
namespace xcl {
class ConcurrentLinkedQueueImpl {
  class Node;
  class TaggedNodePtr : public TaggedPtr {
  public:
    TaggedNodePtr(Node *node, unsigned long tag);

  public:
    Node *GetNode() const;
    void SetNode(Node *node);
    TaggedNodePtr GetNextPtr() const;
    Node *operator->() const;
  };
  class Node {
    std::atomic<TaggedNodePtr> atomic_next_;

  public:
    TaggedNodePtr GetNext() const;
    void SetNext(const TaggedNodePtr &tagged_node_ptr);
    bool CasNext(bool weak, TaggedNodePtr &expect, const TaggedNodePtr &update);
  };
  class NodeStack {
    ThreadLocal local_mem_pool_;
    struct FreeNode {
      TaggedPtr next;
    };
    std::atomic<TaggedPtr> atomic_top_;
    class MemoryPool {
      unsigned long total_pool_size_ = 0;
      struct Block {
        void *address;
        unsigned block_size;
        Block *next_block;
      };
      Block *head_block_ = nullptr;
      Block *tail_block_ = nullptr;
      struct Cursor {
        Block *block = nullptr;
        char *pos = nullptr;
      };
      Cursor cursor_;

    public:
      void *Alloc();
      void Release();
      bool Available() const;

    public:
      ~MemoryPool();
    };

  public:
    Node *Get();
    void Put(Node *node);
  };
  NodeStack node_stack_;
  std::atomic<TaggedNodePtr> atomic_head_{};
  std::atomic<TaggedNodePtr> atomic_tail_{};

public:
  ConcurrentLinkedQueueImpl();
  ~ConcurrentLinkedQueueImpl();

public:
  bool Offer(const Object &value, Object *(*value_func)(Node *));
  bool Poll(Object *o, Object *(*value_func)(Node *));
  unsigned GetQueueSize() const;
  bool IsQueueEmpty() const;
  void ClearQueue();
};
} // namespace xcl

#endif // SCL_SRC_PUBLIC_SCL_UTIL_CONCURRENT_QUEUES_IMPL_CONCURRENT_LINKED_QUEUE_IMPL_H_
