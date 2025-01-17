/**
 * @author xuyan
 * @brief simple c rbtree implementation with high performance
 * @version 0.1
 * @date 2022-08-05
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/define.h>

struct RbNode {
  struct RbNode *left, *right;
  struct {
    uintptr_t compressed_par : PTR_BITS - 1;
    uintptr_t color : 1;
  };
};
typedef enum { RED = 0, BLACK } RbColor;

static inline always_inline void* RbNode_value(struct RbNode* node) { return node + 1; }
struct RbNode* Rb_parent(struct RbNode* node);
void Rb_setPar(struct RbNode* node, struct RbNode* parent);
void Rb_init(struct RbNode* header);
struct RbNode* Rb_root(struct RbNode* header);
struct RbNode* Rb_rightMost(struct RbNode* node);
struct RbNode* Rb_leftMost(struct RbNode* node);
void Rb_add(bool left, struct RbNode* node, struct RbNode* par, struct RbNode* header);
struct RbNode* Rb_max(struct RbNode* header);
struct RbNode* Rb_min(struct RbNode* header);
struct RbNode* Rb_remove(struct RbNode* node, struct RbNode* header);
struct RbNode* Rb_next(struct RbNode* node, struct RbNode* header);
struct RbNode* Rb_prev(struct RbNode* node, struct RbNode* header);
bool Rb_empty(struct RbNode* header);

#ifdef __cplusplus
}
#endif
