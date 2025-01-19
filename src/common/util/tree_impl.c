//
// Created by 14049 on 2024/6/24.
//

#include <internal/util/tree_impl.h>
#include <string.h>
#include <xcl/util/log.h>

static inline int TreeImpl_compare(struct TreeImpl* tree_impl, const void* o1, const void* o2) {
  return tree_impl->key_comp ? tree_impl->key_comp->__vptr->compare(tree_impl->key_comp, o1, o2)
                             : memcmp(o1, o2, tree_impl->key_size);
}
static inline const void* TreeImpl_key(struct TreeImpl* tree_impl, const void* item) {
  return tree_impl->key_handler ? tree_impl->key_handler(item) : item;
}
static struct RbNode* TreeImpl_newNode(struct TreeImpl* tree_impl) {
  struct RbNode* node = Pool_alloc(tree_impl->pool, sizeof(struct RbNode) + tree_impl->item_size);
  if (node)
    Rb_init(node);
  return node;
}
static inline void TreeImpl_freeNode(struct TreeImpl* tree_impl, struct RbNode* node) {
  Pool_dealloc(tree_impl->pool, node);
}
static void TreeImpl_freeTree(struct TreeImpl* tree_impl, struct RbNode* node) {
  if (node) {
    TreeImpl_freeTree(tree_impl, node->left);
    TreeImpl_freeTree(tree_impl, node->right);
    TreeImpl_freeNode(tree_impl, node);
  }
}
static struct RbNode* TreeImpl_cloneNode(struct TreeImpl* tree_impl, struct RbNode* node) {
  struct RbNode* new_node = TreeImpl_newNode(tree_impl);
  if (new_node) {
    new_node->color = node->color;
    memcpy(new_node + 1, node + 1, tree_impl->item_size);
  }
  return new_node;
}
static struct RbNode* TreeImpl_copyTree(struct TreeImpl* tree_impl, struct RbNode* top,
                                        struct RbNode* par) {
  struct RbNode* node = NULL;
  if (top && (node = TreeImpl_cloneNode(tree_impl, top)) &&
      (!top->left || (node->left = TreeImpl_copyTree(tree_impl, top->left, node))) &&
      (!top->right || (node->right = TreeImpl_copyTree(tree_impl, top->right, node)))) {
    Rb_setPar(node, par);
    return node;
  }
  TreeImpl_freeTree(tree_impl, node);
  return NULL;
}
static inline struct RbNode* TreeImpl_root(struct TreeImpl* tree_impl) {
  return Rb_parent(&tree_impl->header);
}
struct RbNode* TreeImpl_lower(struct TreeImpl* tree_impl, const void* key) {
  struct RbNode* lb = &tree_impl->header;
  struct RbNode* cur = TreeImpl_root(tree_impl);
  while (cur) {
    if (TreeImpl_compare(tree_impl, key, TreeImpl_key(tree_impl, cur + 1)) <= 0) {
      lb = cur;
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }
  return lb;
}
struct RbNode* TreeImpl_upper(struct TreeImpl* tree_impl, const void* key) {
  struct RbNode* ub = &tree_impl->header;
  struct RbNode* cur = TreeImpl_root(tree_impl);
  while (cur) {
    if (TreeImpl_compare(tree_impl, key, TreeImpl_key(tree_impl, cur + 1)) < 0) {
      ub = cur;
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }
  return ub;
}
void TreeImpl_init(struct TreeImpl* tree_impl, int key_size, unsigned item_size,
                   Comparator* key_comp, const void* (*key_handler)(const void*)) {
  LOG_ASSERT_P(key_comp || key_size > 0);
  memset(tree_impl, 0, sizeof(*tree_impl));
  tree_impl->key_comp = key_comp;
  tree_impl->key_size = key_size;
  tree_impl->item_size = item_size;
  Rb_init(&tree_impl->header);
  tree_impl->header.left = tree_impl->header.right = &tree_impl->header;
  tree_impl->key_handler = key_handler;
}
bool TreeImpl_copy(struct TreeImpl* copy, struct TreeImpl* tree_impl) {
  if (!tree_impl->size) {
    copy->size = 0;
    copy->header.left = copy->header.right = &copy->header;
    Rb_setPar(&copy->header, NULL);
    return true;
  }
  struct RbNode* root = TreeImpl_copyTree(copy, Rb_parent(&tree_impl->header), &copy->header);
  if (root) {
    Rb_setPar(&copy->header, root);
    copy->header.left = Rb_leftMost(root);
    copy->header.right = Rb_rightMost(root);
    copy->size = tree_impl->size;
    return true;
  }
  TreeImpl_clear(copy);
  return false;
}
void TreeImpl_move(struct TreeImpl* tree_impl, struct TreeImpl* obj) {
  if (obj->size) {
    tree_impl->header = obj->header;
    tree_impl->size = obj->size;
    obj->size = 0;
    obj->header.left = obj->header.right = &obj->header;
    Rb_setPar(&obj->header, NULL);
  }
}
struct RbNode* TreeImpl_add(struct TreeImpl* tree_impl, bool unique, const void* item) {
  struct RbNode* node = TreeImpl_reserve(tree_impl, unique, TreeImpl_key(tree_impl, item));
  if (node)
    memcpy(node + 1, item, tree_impl->item_size);
  return node;
}
struct RbNode* TreeImpl_reserve(struct TreeImpl* tree_impl, bool unique, const void* key) {
  struct RbNode* par = &tree_impl->header;
  int cmp_ret = -1;
  for (struct RbNode* cur = Rb_parent(par); cur;) {
    cmp_ret = TreeImpl_compare(tree_impl, key, TreeImpl_key(tree_impl, cur + 1));
    if (cmp_ret == 0 && unique)
      return NULL;
    par = cur;
    cur = cmp_ret < 0 ? cur->left : cur->right;
  }
  struct RbNode* node = TreeImpl_newNode(tree_impl);
  if (!node)
    return NULL;
  Rb_add(cmp_ret < 0, node, par, &tree_impl->header);
  ++tree_impl->size;
  return node;
}
void TreeImpl_remove(struct TreeImpl* tree_impl, struct RbNode* node) {
  node = Rb_remove(node, &tree_impl->header);
  TreeImpl_freeNode(tree_impl, node);
  --tree_impl->size;
}
struct RbNode* TreeImpl_pop(struct TreeImpl* tree_impl, struct RbNode* node) {
  node = Rb_remove(node, &tree_impl->header);
  --tree_impl->size;
  return node;
}
struct RbNode* TreeImpl_find(struct TreeImpl* tree_impl, const void* key) {
  struct RbNode* cur = TreeImpl_root(tree_impl);
  while (cur) {
    int cmp_ret = TreeImpl_compare(tree_impl, key, TreeImpl_key(tree_impl, cur + 1));
    if (cmp_ret == 0)
      break;
    cur = cmp_ret < 0 ? cur->left : cur->right;
  }
  return cur;
}
void TreeImpl_clear(struct TreeImpl* tree_impl) {
  TreeImpl_freeTree(tree_impl, Rb_parent(&tree_impl->header));
  tree_impl->header.left = tree_impl->header.right = &tree_impl->header;
  Rb_setPar(&tree_impl->header, NULL);
  tree_impl->size = 0;
}
void TreeImpl_swap(struct TreeImpl* impl1, struct TreeImpl* impl2) {
  if (impl1 && impl2) {
    struct TreeImpl tmp;
    TreeImpl_move(&tmp, impl1);
    TreeImpl_move(impl1, impl2);
    TreeImpl_move(impl2, &tmp);
  }
}
static int TreeImpl_black(struct TreeImpl* tree_impl, struct RbNode* node) {
  int n = 0;
  while (node != &tree_impl->header) {
    if (node->color == BLACK)
      ++n;
    node = Rb_parent(node);
  }
  return n;
}
static bool TreeImpl_verifyNode(struct TreeImpl* tree_impl, struct RbNode* node, int blacks) {
  struct RbNode *left_child = node->left, *right_child = node->right;
  if (!left_child && !right_child)
    return TreeImpl_black(tree_impl, node) == blacks;
  if (node->color == RED) {
    if (left_child && left_child->color == RED)
      return false;
    if (right_child && right_child->color == RED)
      return false;
  }
  if (left_child && TreeImpl_compare(tree_impl, TreeImpl_key(tree_impl, left_child + 1),
                                     TreeImpl_key(tree_impl, node + 1)) > 0)
    return false;
  if (right_child && TreeImpl_compare(tree_impl, TreeImpl_key(tree_impl, right_child + 1),
                                      TreeImpl_key(tree_impl, node + 1)) < 0)
    return false;
  return true;
}
static bool TreeImpl_valid(struct TreeImpl* tree_impl) {
  struct RbNode* header = &tree_impl->header;
  if (Rb_parent(header) == NULL)
    return header->left == header && header->right == header && header->color == RED &&
           tree_impl->size == 0;
  struct RbNode* iter;
  iter = header->left;
  int blacks = TreeImpl_black(tree_impl, iter);
  for (; iter != header; iter = Rb_next(iter, header)) {
    if (!TreeImpl_verifyNode(tree_impl, iter, blacks))
      return false;
  }
  struct RbNode* root = Rb_parent(header);
  if (header->left != Rb_leftMost(root))
    return false;
  if (header->right != Rb_rightMost(root))
    return false;
  return true;
}
static bool TreeImpl_treeEqual(struct TreeImpl* tree_impl, struct RbNode* top1,
                               struct RbNode* top2) {
  if (top1 == top2)
    return true;
  if (!top1 || !top2)
    return false;
  if (top1->color != top2->color || TreeImpl_compare(tree_impl, TreeImpl_key(tree_impl, top1 + 1),
                                                     TreeImpl_key(tree_impl, top2 + 1)))
    return false;
  return TreeImpl_treeEqual(tree_impl, top1->left, top2->left) &&
         TreeImpl_treeEqual(tree_impl, top1->right, top2->right);
}
void TreeImpl_verify(struct TreeImpl* tree_impl) { LOG_ASSERT_P(TreeImpl_valid(tree_impl)); }
bool TreeImpl_equal(struct TreeImpl* impl1, struct TreeImpl* impl2) {
  return impl1->item_size == impl2->item_size &&
         TreeImpl_treeEqual(impl1, TreeImpl_root(impl1), TreeImpl_root(impl2));
}
struct RbNode* TreeImpl_header(struct TreeImpl* impl) { return &impl->header; }

TreeIter TreeImpl_begin(struct TreeImpl* impl) { return TreeIter_make(impl->header.left, impl); }
TreeIter TreeImpl_end(struct TreeImpl* impl) { return TreeIter_make(&impl->header, impl); }
