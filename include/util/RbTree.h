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

#include "lang/xcldef.h"
#include <stdint.h>
#include <stdbool.h>
#include <pool/pool.h>

typedef enum
{
    RED,
    BLACK
} CRbColor;

typedef struct _CRbNode_st CRbNode;
struct _CRbNode_st {
    CRbNode *left, *right;
    uintptr_t parAndColor;
    char attach[0];
};

typedef struct {
    int32_t count;
    const int32_t eleSize;
    CPool* const pool;
    CRbNode header;
} CRbTree;

/**
 * @brief alloc tree node
 * @param tree rbtree object
 * @return tree node object
 * @author xuyan
 * @date 2022-08-05
 */
CRbNode*
__RbTree_allocNode(CRbTree* tree);

/**
 * @brief dealloc tree node
 * @param tree rbtree object
 * @param node tree node pointer
 * @author xuyan
 * @date 2022-08-05
 */
void
__RbTree_deallocNode(CRbTree* tree, CRbNode* node);

/**
 * @brief create rbtree
 * @param es element size
 * @param pool CPool object
 * @return tree object
 * @author xuyan
 * @date 2022-08-08
 */
CRbTree
RbTree_make(int32_t es, CPool* pool);

/**
 * @brief initialize tree object
 * @param tree rbtree object
 * @return true if initialize tree successfully, otherwise false
 * @author xuyan
 * @date 2022-08-08
 */
bool
RbTree_init(CRbTree* tree);

/**
 * @brief use destructor to release rbtree memory resource
 * @param tree rbtree object
 * @param destructor function destruct tree node data
 * @param usr user param for destructor
 * @author xuyan
 * @date 2022-08-06
 */
void
RbTree_release(CRbTree* tree, void (*destructor)(void*, void*), void* usr);

/**
 * @brief rbtree is empty
 * @param tree rbtree object
 * @return true if tree is empty, otherwise false
 * @author xuyan
 * @date 2022-08-06
 */
bool
RbTree_empty(CRbTree* tree);

/**
 * @brief add tree node to rbtree
 * @param tree tree object
 * @param left add tree node as left child of par
 * @param par parent of new node added
 * @param newNode new node added
 * @return true if add successfully, otherwise false
 * @author xuyan
 * @date 2022-08-06
 */
bool
_RbTree_addNode(CRbTree* tree, bool left, CRbNode* par, CRbNode* newNode);

/**
 * @brief remove tree node from rbtree
 * @param tree tree object
 * @param node node required remove
 * @return node removed if remove successfully, otherwise NULL
 * @author xuyan
 * @date 2022-08-06
 */
CRbNode*
_RbTree_rmNode(CRbTree* tree, CRbNode* node);

/**
 * @brief find tree node which data equals to src
 * @param tree tree object
 * @param src source data
 * @param cmp comparator used to compare data
 * @param usr user defined object for compare
 * @return tree node found, otherwise NULL
 * @author xuyan
 * @date 2022-08-06
 */
CRbNode*
_RbTree_find(CRbTree* tree,
             const void* src,
             int32_t (*cmp)(void*, const void*, const void*),
             void* usr);

/**
 * @brief find a add position for src
 * @param tree tree object
 * @param interruptIfFound whether search should be interrupted
 * if tree has tree node which contains value equals to src
 * @param src source element
 * @param cmp comparator used to compare data
 * @param usr user defined object for compare
 * @param left true if src less equal than value which return node contains
 * otherwise false
 * @return tree node which should be parent of src
 * @author xuyan
 * @date 2022-08-06
 */
CRbNode*
__RbTree_findAddPos(CRbTree* tree,
                    bool interruptIfFound,
                    const void* src,
                    int32_t (*cmp)(void*, const void*, const void*),
                    void* usr,
                    bool* left);

/**
 * @brief find range [lower,upper) at tree object
 * lower is the largest node which data <= src
 * upper is the first node which data > src
 * @param tree tree object
 * @param src source element
 * @param cmp comparator used to compare data
 * @param usr user defined object for compare
 * @param lower output pointer store lower node address
 * @param upper output pointer store upper node address
 * @author xuyan
 * @date 2022-08-06
 */
void
RbTree_range(CRbTree* tree,
             const void* src,
             int32_t (*cmp)(void*, const void*, const void*),
             void* usr,
             CRbNode** lower,
             CRbNode** upper);

/**
 * @brief first node of tree
 * @param tree tree object
 * @return node at begin position
 * @author xuyan
 * @date 2022-08-06
 */
CRbNode*
RbTree_begin(CRbTree* tree);

/**
 * @brief last node of tree
 * @param tree tree object
 * @return node at end position
 * @author xuyan
 * @date 2022-08-06
 */
CRbNode*
RbTree_end(CRbTree* tree);

/**
 * @brief node at next position
 * @param tree tree object
 * @param node current node
 * @return next node
 * @author xuyan
 * @date 2022-08-06
 */
CRbNode*
RbTree_next(CRbTree* tree, CRbNode* node);

/**
 * @brief get previous position node of tree
 * @param tree tree object
 * @param node current node
 * @return prev node
 * @author xuyan
 * @date 2022-08-06
 */
CRbNode*
RbTree_prev(CRbTree* tree, CRbNode* node);

/**
 * @brief make rbtree copy
 * note that if tree is not completely copied,
 * use RbTree_release to completely release dst rbtree
 * dest rbtree must be empty, otherwise return false
 * @param tree source rbtree object
 * @param constructor construct function used to construct attach of tree node
 * @param usr user defined object for construct
 * @param dst output rbtree object
 * @return true if copy completely, otherwise false
 * @author xuyan
 * @date 2022-08-06
 */
bool
__RbTree_copy(CRbTree* tree,
              bool (*constructor)(void*, void*, const void*),
              void* usr,
              CRbTree* dst);

/**
 * @brief verify if rbtree is conform the red-black-tree definition
 * @param tree rbtree object
 * @param cmp comparator used to compare
 * @param usr user defined object for compare
 * @return true if rbtree is valid, otherwise false
 * @author xuyan
 * @date 2022-08-06
 */
bool
__RbTree_verify(CRbTree* tree,
                int32_t (*cmp)(void*, const void*, const void*),
                void* usr);

#ifdef __cplusplus
}
#endif
