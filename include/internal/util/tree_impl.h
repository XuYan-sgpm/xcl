//
// Created by 14049 on 2024/6/24.
//

#pragma once

#include <internal/util/rb.h>
#include <stdbool.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/comparator.h>
#include <xcl/util/tree_iter.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _aligned(64) TreeImpl {
  struct RbNode header;
  unsigned size;
  int key_size;
  unsigned item_size;
  Comparator* key_comp;
  struct Pool* pool;
  const void* (*key_handler)(const void*);
};

struct RbNode* TreeImpl_lower(struct TreeImpl* tree_impl, const void* key);
struct RbNode* TreeImpl_upper(struct TreeImpl* tree_impl, const void* key);
void TreeImpl_init(struct TreeImpl* tree_impl, int key_size, unsigned item_size,
                   Comparator* key_comp, const void* (*key_handler)(const void*));
bool TreeImpl_copy(struct TreeImpl* copy, struct TreeImpl* tree_impl);
void TreeImpl_move(struct TreeImpl* tree_impl, struct TreeImpl* obj);
struct RbNode* TreeImpl_reserve(struct TreeImpl* tree_impl, bool unique, const void* key);
struct RbNode* TreeImpl_add(struct TreeImpl* tree_impl, bool unique, const void* item);
void TreeImpl_remove(struct TreeImpl* tree_impl, struct RbNode* node);
struct RbNode* TreeImpl_pop(struct TreeImpl* tree_impl, struct RbNode* node);
struct RbNode* TreeImpl_find(struct TreeImpl* tree_impl, const void* key);
void TreeImpl_clear(struct TreeImpl* tree_impl);
void TreeImpl_swap(struct TreeImpl* impl1, struct TreeImpl* impl2);
void TreeImpl_verify(struct TreeImpl* tree_impl);
bool TreeImpl_equal(struct TreeImpl* impl1, struct TreeImpl* impl2);
struct RbNode* TreeImpl_header(struct TreeImpl* impl);
TreeIter TreeImpl_begin(struct TreeImpl* impl);
TreeIter TreeImpl_end(struct TreeImpl* impl);

#ifdef __cplusplus
}
#endif
