//
// Created by 徐琰 on 2021/12/29.
//

#ifndef SCL_MULTI_SET_H
#define SCL_MULTI_SET_H

#include "basic_rb_tree.h"
#include "include/xcl/lang/def_obj_interface.h"
#include "include/xcl/util/comparator.h"

namespace xcl {
template <typename E> class MultiSet final {
private:
  struct Node {
    char data[32];

    E value;
  };

private:
  class TreeContext final : public BasicRbTree {
    friend class MultiSet<E>;

    typedef MultiSet<E>::Node Node;

    Comparator<E> &comparator_;

    Allocator &allocator_;

  protected:
    RbNode *ImplAllocRbNode(const void *src) override;

    RbNode *ImplCloneRbNode(const RbNode *node) override;

    void ImplReleaseRbNode(RbNode *node) override;

    void ImplDeallocRbNode(RbNode *node) override;

  public:
    explicit TreeContext(Allocator &allocator, Comparator<E> &comparator);
  };

  TreeContext *context_;

private:
  static int CompareValue(void *args, RbNode *node, const void *src);

  static int CompareNode(void *args, RbNode *node1, RbNode *node2);

public:
  explicit MultiSet(Comparator<E> &comparator);

  MultiSet(Allocator &allocator, Comparator<E> &comparator);

public:
  bool Verify() const;

public:
  bool Add(const E &e);

  unsigned Size() const;

  void Clear();

  E &Find(const E &e) const;
};

template <typename E>
RbNode *MultiSet<E>::TreeContext::ImplAllocRbNode(const void *src) {
  void *p = allocator_.Allocate(sizeof(Node));
  if (!p) {
    return nullptr;
  }
  DefObjInterface<E>::instance().CopyConstruct(&((Node *)p)->value,
                                               (const E *)src);
  return (RbNode *)p;
}

template <typename E>
RbNode *MultiSet<E>::TreeContext::ImplCloneRbNode(const RbNode *node) {
  return ImplAllocRbNode(&((const Node *)node)->value);
}

template <typename E>
void MultiSet<E>::TreeContext::ImplReleaseRbNode(RbNode *node) {
  DefObjInterface<E>::instance().destroy(&((Node *)node)->value, 0, nullptr);
}

template <typename E>
void MultiSet<E>::TreeContext::ImplDeallocRbNode(RbNode *node) {
  allocator_.Deallocate(node, sizeof(Node));
}

template <typename E>
MultiSet<E>::TreeContext::TreeContext(Allocator &allocator,
                                      Comparator<E> &comparator)
    : BasicRbTree(), allocator_(allocator), comparator_(comparator) {}

template <typename E>
MultiSet<E>::MultiSet(Comparator<E> &comparator)
    : context_(new TreeContext(SysAllocator::instance(), comparator)) {}

template <typename E>
MultiSet<E>::MultiSet(Allocator &allocator, Comparator<E> &comparator)
    : context_(new TreeContext(allocator, comparator)) {}

template <typename E> bool MultiSet<E>::Verify() const {
  return context_->verify(&CompareNode);
}

template <typename E> bool MultiSet<E>::Add(const E &e) {
  return context_->add(&e, &CompareValue);
}

template <typename E> unsigned MultiSet<E>::Size() const {
  return context_->size();
}

template <typename E>
int MultiSet<E>::CompareValue(void *args, RbNode *node, const void *src) {
  return ((TreeContext *)args)
      ->comparator_(((Node *)node)->value, *(const E *)src);
}

template <typename E>
int MultiSet<E>::CompareNode(void *args, RbNode *node1, RbNode *node2) {
  return ((TreeContext *)args)
      ->comparator_(((Node *)node1)->value, ((Node *)node2)->value);
}

template <typename E> void MultiSet<E>::Clear() { context_->clear(); }

template <typename E> E &MultiSet<E>::Find(const E &e) const {
  auto *node = context_->find(&e, &CompareValue);
  return ((Node *)node)->value;
}
} // namespace xcl

#endif // SCL_MULTI_SET_H
