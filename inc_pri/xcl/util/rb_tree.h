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

#include "xcl/lang/xcl_def.h"
#include <stdint.h>
#include <stdbool.h>
#include <xcl/pool/pool.h>

typedef enum RbColor
{
    RED,
    BLACK
} RbColor;

typedef struct RbNode {
    struct RbNode *left, *right;
    uintptr_t par_and_color;
} RbNode;

typedef struct RbTree RbTree;

struct RbTree {
    int32_t count;
    Pool* const pool;
    RbNode header;
};

/**
 * @brief alloc rbnode with pool of rbtree
 * @param tree rbtree object
 * @param element_size element data size
 * @return rbnode if success, otherwise NULL
 */
RbNode*
__RbTree_allocNode(RbTree* tree, int32_t element_size);

/**
 * @brief dealloc rbnode with pool of rbtree
 * @param tree rbtree object
 * @param node rbnode
 * @param element_size element data size
 */
void
__RbTree_deallocNode(RbTree* tree, RbNode* node, int32_t element_size);

/**
 * @brief root node of rbtree
 * @param tree rbtree object
 * @return root node if rbtree is not empty, otherwise NULL
 */
RbNode*
__RbTree_root(RbTree* tree);

/**
 * @brief init rbtree
 * @param tree rbtree
 * @param pool c pool object
 */
void
__RbTree_init(RbTree* tree, Pool* pool);

/**
 * @brief new rbtree object
 * @param pool c pool
 * @return rbtree if success, otherwise NULL
 */
RbTree*
__RbTree_new(Pool* pool);

/**
 * @brief add rbnode into rbtree
 * @param tree rbtree
 * @param left whether rbnode should be left child of its parent
 * @param par rbnode parent
 * @param new_node the rbnode to add
 * @return true if success, otherwise false
 */
bool
__RbTree_addNode(RbTree* tree, bool left, RbNode* par, RbNode* new_node);

/**
 * @brief remove rbnode from rbtree
 * @param tree rbtree
 * @param node the rbnode to remove
 * @return removed rbnode, usually same as node
 */
RbNode*
__RbTree_removeNode(RbTree* tree, RbNode* node);

/**
 * @brief rbtree is empty
 * @param tree rbtree object
 * @return true if tree is empty, otherwise false
 * @author xuyan
 * @date 2022-08-06
 */
bool
__RbTree_empty(RbTree* tree);

/**
 * @brief first node of tree
 * @param tree tree object
 * @return node at begin position
 * @author xuyan
 * @date 2022-08-06
 */
RbNode*
__RbTree_begin(RbTree* tree);

/**
 * @brief last node of tree
 * @param tree tree object
 * @return node at end position
 * @author xuyan
 * @date 2022-08-06
 */
RbNode*
__RbTree_end(RbTree* tree);

/**
 * @brief node at next position
 * @param tree tree object
 * @param node current node
 * @return next node
 * @author xuyan
 * @date 2022-08-06
 */
RbNode*
__RbTree_next(RbTree* tree, RbNode* node);

/**
 * @brief get previous position node of tree
 * @param tree tree object
 * @param node current node
 * @return prev node
 * @author xuyan
 * @date 2022-08-06
 */
RbNode*
__RbTree_prev(RbTree* tree, RbNode* node);

#ifdef __cplusplus
}
#endif
