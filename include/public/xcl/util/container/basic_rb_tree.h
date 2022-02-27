//
// Created by 徐琰 on 2021/12/29.
//

#ifndef SCL_BASIC_RB_TREE_H
#define SCL_BASIC_RB_TREE_H

namespace xcl {
struct RbNode;
struct RbHeader {
  RbNode *const node;

 public:
  explicit RbHeader(RbNode *node);
  ~RbHeader();
};

class BasicRbTree {
 private:
  RbHeader h_;
  unsigned node_count_ = 0;

 private:
  static RbNode *
  MakeHeader();
  RbNode *
  GetRoot() const;
  void
  SetRoot(RbNode *root);

 public:
  explicit BasicRbTree();
  BasicRbTree(const BasicRbTree &t);
  BasicRbTree(BasicRbTree &&) noexcept;
  ~BasicRbTree();

 protected:
  virtual RbNode *
  ImplAllocRbNode(const void *src) = 0;
  virtual RbNode *
  ImplCloneRbNode(const RbNode *node) = 0;
  virtual void
  ImplReleaseRbNode(RbNode *node) = 0;
  virtual void
  ImplDeallocRbNode(RbNode *node) = 0;

  RbNode *
  Minimum() const;
  RbNode *
  Maximum() const;
  RbNode *
  Header() const;
  void
  LowerBound(const void *src, RbNode *&node,
			 int (*cmp)(void *, RbNode *, const void *)) const;
  void
  UpperBound(const void *src, RbNode *&node,
			 int (*cmp)(void *, RbNode *, const void *)) const;
  bool
  Verify(int (*cmp)(void *, RbNode *, RbNode *)) const;

 protected:
  bool
  UniqueAdd(const void *src, int (*cmp)(void *, RbNode *, const void *));
  bool
  UniqueAdd(RbNode *hint, const void *src,
			int (*cmp)(void *, RbNode *, const void *));
  bool
  Add(const void *src, int (*cmp)(void *, RbNode *, const void *));
  bool
  Add(RbNode *hint, const void *src,
	  int (*cmp)(void *, RbNode *, const void *));
  bool
  Remove(const void *src, int (*cmp)(void *, RbNode *, const void *));
  bool
  Remove(RbNode *node);
  void
  Clear();
  unsigned
  Size() const;
  bool
  IsEmpty() const;
  RbNode *
  Find(const void *src, int (*cmp)(void *, RbNode *, const void *));

 public:
  BasicRbTree &
  operator=(const BasicRbTree &t);
  BasicRbTree &
  operator=(BasicRbTree &&t) noexcept;

 private:
  static void
  DropRbNode(void *arg, RbNode *node);
  static RbNode *
  CloneRbNode(void *arg, RbNode *node);
  static RbNode *
  AllocRbNode(void *arg, const void *src);
};
}// namespace xcl

#endif// SCL_BASIC_RB_TREE_H
