//
// Created by 14049 on 2024/6/24.
//

#include <internal/util/rb.h>
#include <internal/util/tree_impl.h>
#include <string.h>
#include <xcl/util/tree_iter.h>

TreeIter TreeIter_make(void* node, void* impl) {
  TreeIter it = {.cur = node, .impl = impl};
  return it;
}
TreeIter TreeIter_next(TreeIter it) {
  return TreeIter_make(Rb_next(it.cur, TreeImpl_header(it.impl)), it.impl);
}
TreeIter TreeIter_prev(TreeIter it) {
  return TreeIter_make(Rb_prev(it.cur, TreeImpl_header(it.impl)), it.impl);
}
bool TreeIter_equal(TreeIter it1, TreeIter it2) { return memcmp(&it1, &it2, sizeof(it1)) == 0; }
bool TreeIter_valid(TreeIter it) {
  return it.cur && it.impl && it.cur != &((struct TreeImpl*)it.impl)->header;
}
void* TreeIter_key(TreeIter it) {
  return !it.cur || !it.impl ? NULL
         : ((struct TreeImpl*)it.impl)->key_handler
             ? (void*)((struct TreeImpl*)it.impl)->key_handler(RbNode_value(it.cur))
             : RbNode_value(it.cur);
}
void* TreeIter_value(TreeIter it) {
  char* key = TreeIter_key(it);
  return !key ? NULL
         : ((struct TreeImpl*)it.impl)->key_size == ((struct TreeImpl*)it.impl)->item_size
             ? key
             : key + ((struct TreeImpl*)it.impl)->key_size;
}
