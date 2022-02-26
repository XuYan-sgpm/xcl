//
// Created by 徐琰 on 2021/12/29.
//
#include <xcl/util/container/basic_rb_tree.h>
#include <xcl/util/container/rb_core.h>
#include <xcl/util/container/rb_def.h>
namespace xcl {
RbHeader::RbHeader(RbNode *node) : node(node) {}
RbHeader::~RbHeader() { delete node; }

RbNode *BasicRbTree::MakeHeader() {
  auto *header = new RbNode();
  header->left = header->right = header;
  header->par = nullptr;
  header->color = RED;
  return header;
}
RbNode *BasicRbTree::GetRoot() const { return h_.node->par; }
void BasicRbTree::SetRoot(RbNode *root) { h_.node->par = root; }
BasicRbTree::BasicRbTree() : h_(MakeHeader()) {}
BasicRbTree::BasicRbTree(const BasicRbTree &t) : h_(MakeHeader()) {
  if (!t.IsEmpty()) {
    SetRoot(RbCopy(t.GetRoot(), Header(), {(void *)this, &CloneRbNode},
                   {(void *)this, &DropRbNode}));
    h_.node->left = RbMinimum(GetRoot());
    h_.node->right = RbMaximum(GetRoot());
    node_count_ = t.node_count_;
  }
}
BasicRbTree::BasicRbTree(BasicRbTree &&t) noexcept : h_(MakeHeader()) {
  SetRoot(t.GetRoot());
  h_.node->left = t.h_.node->left;
  h_.node->right = t.h_.node->right;
  node_count_ = t.node_count_;
  t.SetRoot(nullptr);
  t.h_.node->left = t.h_.node;
  t.h_.node->right = t.h_.node;
  t.node_count_ = 0;
}
BasicRbTree::~BasicRbTree() { Clear(); }
RbNode *BasicRbTree::Minimum() const { return h_.node->left; }
RbNode *BasicRbTree::Maximum() const { return h_.node->right; }
RbNode *BasicRbTree::Header() const { return h_.node; }
void BasicRbTree::LowerBound(const void *src, RbNode *&node,
                             int (*cmp)(void *, RbNode *, const void *)) const {
  node = RbFindLowerBound(src, h_.node, {(void *)this, cmp});
  if (!node) {
    node = Header();
  }
}
void BasicRbTree::UpperBound(const void *src, RbNode *&node,
                             int (*cmp)(void *, RbNode *, const void *)) const {
  node = RbFindUpperBound(src, h_.node, {(void *)this, cmp});
  if (!node) {
    node = Header();
  }
}
bool BasicRbTree::Verify(int (*cmp)(void *, RbNode *, RbNode *)) const {
  return RbVerify(Header(), {(void *)this, cmp});
}
bool BasicRbTree::UniqueAdd(const void *src,
                            int (*cmp)(void *, RbNode *, const void *)) {
  return RbUniqueAdd(src, Header(), {(void *)this, cmp},
                     {(void *)this, &AllocRbNode});
}
bool BasicRbTree::UniqueAdd(RbNode *hint, const void *src,
                            int (*cmp)(void *, RbNode *, const void *)) {
  return RbUniqueAdd(hint, src, Header(), {(void *)this, cmp},
                     {(void *)this, &AllocRbNode});
}
bool BasicRbTree::Add(const void *src,
                      int (*cmp)(void *, RbNode *, const void *)) {
  return RbAdd(src, Header(), {(void *)this, cmp},
               {(void *)this, &AllocRbNode});
}
bool BasicRbTree::Add(RbNode *hint, const void *src,
                      int (*cmp)(void *, RbNode *, const void *)) {
  return RbAdd(hint, src, Header(), {(void *)this, cmp},
               {(void *)this, &AllocRbNode});
}
bool BasicRbTree::Remove(const void *src,
                         int (*cmp)(void *, RbNode *, const void *)) {
  return RbRemove(src, Header(), {(void *)this, cmp},
                  {(void *)this, &DropRbNode});
}
bool BasicRbTree::Remove(RbNode *node) {
  if (node) {
    auto *del_node = RbRemoveNode(node, Header());
    ImplReleaseRbNode(del_node);
    ImplDeallocRbNode(del_node);
    --node_count_;
  }
  return true;
}
void BasicRbTree::Clear() {
  if (!IsEmpty()) {
    RbClear(GetRoot(), {(void *)this, &DropRbNode});
    SetRoot(nullptr);
    h_.node->left = h_.node;
    h_.node->right = h_.node;
  }
}
unsigned BasicRbTree::Size() const { return node_count_; }
bool BasicRbTree::IsEmpty() const {
  return node_count_ == 0 && !h_.node->par && h_.node->left == h_.node &&
         h_.node->right == h_.node;
}
RbNode *BasicRbTree::Find(const void *src,
                          int (*cmp)(void *, RbNode *, const void *)) {
  RbNode *par_or_target;
  auto ret = RbFindFirst(src, par_or_target, Header(), {(void *)this, cmp});
  return ret == 0 ? par_or_target : Header();
}
BasicRbTree &BasicRbTree::operator=(const BasicRbTree &t) {
  if (&t != this) {
    if (node_count_) {
      Clear();
    }
    if (t.node_count_) {
      SetRoot(RbCopy(t.GetRoot(), Header(), {(void *)this, &CloneRbNode},
                     {(void *)this, &DropRbNode}));
      if (node_count_) {
        h_.node->left = RbMinimum(GetRoot());
        h_.node->right = RbMaximum(GetRoot());
      }
    }
  }
  return *this;
}
BasicRbTree &BasicRbTree::operator=(BasicRbTree &&t) noexcept {
  if (&t != this) {
    if (node_count_) {
      Clear();
    }
    SetRoot(t.GetRoot());
    h_.node->left = t.h_.node->left;
    h_.node->right = t.h_.node->right;
    node_count_ = t.node_count_;
    t.SetRoot(nullptr);
    t.h_.node->left = t.h_.node;
    t.h_.node->right = t.h_.node;
    t.node_count_ = 0;
  }
  return *this;
}
void BasicRbTree::DropRbNode(void *arg, RbNode *node) {
  auto tree = reinterpret_cast<BasicRbTree *>(arg);
  tree->ImplReleaseRbNode(node);
  tree->ImplDeallocRbNode(node);
  --tree->node_count_;
}
RbNode *BasicRbTree::CloneRbNode(void *arg, RbNode *node) {
  auto tree = reinterpret_cast<BasicRbTree *>(arg);
  ++tree->node_count_;
  return tree->ImplCloneRbNode(node);
}
RbNode *BasicRbTree::AllocRbNode(void *arg, const void *src) {
  auto tree = reinterpret_cast<BasicRbTree *>(arg);
  ++tree->node_count_;
  return tree->ImplAllocRbNode(src);
}
} // namespace xcl