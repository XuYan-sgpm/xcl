/**
 * @author xuyan
 * @brief simple c rbtree implementation with high performance
 * @version 0.1
 * @date 2022-08-05
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/XclDef.h"
#include <stdint.h>
#include <stdbool.h>
#include <xcl/pool/CPool.h>

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
 * @brief alloc rbnode
 * @param tree rbtree object
 * @param size element size
 * @return rbnode object
 * @author xuyan
 * @date 2022-08-05
 */
CRbNode*
__RbTree_allocNode(CRbTree* tree, int32_t size);

/**
 * @brief dealloc rbnode
 * @param tree rbtree object
 * @param node rbnode pointer
 * @param size element size
 * @author xuyan
 * @date 2022-08-05
 */
void
__RbTree_deallocNode(CRbTree* tree, CRbNode* node, int32_t size);

/**
 * @brief create a rbtree object
 * @param es element size
 * @param pool rbtree pool object
 * @return rbtree object
 * @author xuyan
 * @date 2022-08-05
 */
CRbTree
RbTree_make(int32_t es, CPool* pool);

/**
 * @brief use destructor to release rbtree memory resource
 * @param tree rbtree object
 * @param destructor function destruct rbnode data
 * @author xuyan
 * @date 2022-08-06
 */
void
RbTree_release(CRbTree* tree, void (*destructor)(void*));

/**
 * @brief rbtree is empty
 * @param tree rbtree object
 * @return true if tree is empty, otherwise false
 * @author xuyan
 * @date 2022-08-06
 */
bool
RbTree_empty(CRbTree* tree);

#ifdef __cplusplus
}
#endif
