//
// Created by 徐琰 on 2021/12/25.
//

#ifndef SCL_RB_CORE_H
#define SCL_RB_CORE_H

#include "rb_def.h"

namespace xcl {
RbNode *RbMinimum(RbNode *node);
RbNode *RbMaximum(RbNode *node);
RbNode *RbPrev(RbNode *node, RbNode *header);
RbNode *RbNext(RbNode *node, RbNode *header);
void RbLeftRotate(RbNode *node, RbNode *&root);
void RbRightRotate(RbNode *node, RbNode *&root);
void RbAddFixup(RbNode *node, RbNode *&root);
void RbRemoveFixup(RbNode *node, RbNode *par, RbNode *&root);
void RbAddNode(RbNode *&child, RbNode *par, RbNode *node, RbNode *header);
RbNode *RbRemoveNode(RbNode *node, RbNode *header);
namespace rb_pair {
typedef int (*NodeCompareFunc)(void *, RbNode *, RbNode *);
typedef int (*ValueCompareFunc)(void *, RbNode *, const void *);
typedef RbNode *(*NodeAllocFunc)(void *, const void *);
typedef void (*NodeDropFunc)(void *, RbNode *);
typedef RbNode *(*NodeCloneFunc)(void *, RbNode *);

template <typename T> struct Pair {
  void *args;
  const T func;

public:
  Pair(void *a, const T &f) : args(a), func(f) {}
  Pair(const Pair &pair) : args(pair.args), func(pair.func) {}
};
} // namespace rb_pair
bool RbVerify(
    RbNode *header,
    const rb_pair::Pair<rb_pair::NodeCompareFunc> &node_cmp_pair);
RbNode *&RbFind(
    const void *src, RbNode *&par, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair);
int RbFindFirst(
    const void *src, RbNode *&par_or_target, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair);
RbNode *&RbFindHint(
    const void *src, RbNode *&par, RbNode *hint, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair);
int RbFindFirstHint(
    const void *src, RbNode *&par_or_target, RbNode *hint, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair);
RbNode *RbFindUpperBound(
    const void *src, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair);
RbNode *RbFindLowerBound(
    const void *src, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair);
RbNode *
RbAdd(const void *src, RbNode *header,
      const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
      const rb_pair::Pair<rb_pair::NodeAllocFunc> &node_alloc_pair);
RbNode *
RbAdd(RbNode *hint, const void *src, RbNode *header,
      const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
      const rb_pair::Pair<rb_pair::NodeAllocFunc> &node_alloc_pair);
RbNode *RbUniqueAdd(
    const void *src, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
    const rb_pair::Pair<rb_pair::NodeAllocFunc> &node_alloc_pair);
RbNode *RbUniqueAdd(
    RbNode *hint, const void *src, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
    const rb_pair::Pair<rb_pair::NodeAllocFunc> &node_alloc_pair);
bool RbRemove(
    const void *src, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
    const rb_pair::Pair<rb_pair::NodeDropFunc> &node_drop_pair);
void RbClear(RbNode *node,
             const rb_pair::Pair<rb_pair::NodeDropFunc> &node_drop_pair);
RbNode *
RbCopy(RbNode *top, RbNode *par,
       const rb_pair::Pair<rb_pair::NodeCloneFunc> &node_clone_pair,
       const rb_pair::Pair<rb_pair::NodeDropFunc> &node_drop_pair);
} // namespace xcl

#endif // SCL_RB_CORE_H
