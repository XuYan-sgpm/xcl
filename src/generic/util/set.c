//
// Created by 14049 on 2024/6/24.
//

#include <internal/util/rb.h>
#include <internal/util/tree_impl.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/log.h>
#include <xcl/util/set.h>

struct Set {
  struct TreeImpl impl;
};

Set* Set_new(unsigned item_size, Comparator* comparator) {
  Set* set = palloc(sizeof(Set));
  if (set)
    TreeImpl_init(&set->impl, item_size, item_size, comparator, NULL);
  return set;
}
void Set_free(Set* set) {
  if (set) {
    Set_clear(set);
    pfree(set);
  }
}
Set* Set_copy(Set* set) {
  Set* copy = Set_new(set->impl.item_size, set->impl.key_comp);
  if (copy && TreeImpl_copy(&copy->impl, &set->impl))
    return copy;
  Set_free(copy);
  return NULL;
}
bool Set_move(Set* set, Set* obj) {
  if (set->impl.size)
    return false;
  TreeImpl_move(&set->impl, &obj->impl);
  return true;
}
unsigned Set_size(Set* set) { return set->impl.size; }
bool Set_empty(Set* set) { return set->impl.size == 0; }
TreeIter Set_add(Set* set, bool unique, const void* item) {
  struct RbNode* node = TreeImpl_add(&set->impl, unique, item);
  return node ? TreeIter_make(node, set) : Set_end(set);
}
bool Set_remove(Set* set, const void* item) {
  struct RbNode* node = TreeImpl_find(&set->impl, item);
  if (node)
    TreeImpl_remove(&set->impl, node);
  return node;
}
void Set_removeIter(Set* set, TreeIter it) { TreeImpl_remove(&set->impl, it.cur); }
TreeIter Set_begin(Set* set) { return TreeImpl_begin(&set->impl); }
TreeIter Set_end(Set* set) { return TreeImpl_end(&set->impl); }
TreeIter Set_find(Set* set, const void* item) {
  struct RbNode* node = TreeImpl_find(&set->impl, item);
  return TreeIter_make(node ? node : &set->impl.header, set);
}
void Set_range(Set* set, const void* item, TreeIter out[2]) {
  struct RbNode *x = TreeImpl_lower(&set->impl, item), *y = TreeImpl_upper(&set->impl, item);
  out[0] = TreeIter_make(x, set);
  out[1] = TreeIter_make(y, set);
}
void Set_clear(Set* set) { TreeImpl_clear(&set->impl); }
void Set_swap(Set* set1, Set* set2) { TreeImpl_swap(&set1->impl, &set2->impl); }
void Set_verify(Set* set) { TreeImpl_verify(&set->impl); }
bool Set_equal(Set* set1, Set* set2) { return TreeImpl_equal(&set1->impl, &set2->impl); }
