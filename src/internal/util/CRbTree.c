#include "xcl/util/CRbTree.h"
#include <xcl/lang/XclErr.h>
#include <assert.h>
#include <string.h>

CRbNode*
__RbTree_allocNode(CRbTree* tree, int32_t size)
{
    void* p = Pool_alloc(tree->pool, sizeof(CRbNode) + size);
    if (p)
    {
        assert((uintptr_t)p % sizeof(long) == 0);
    }
    return p;
}

void
__RbTree_deallocNode(CRbTree* tree, CRbNode* node, int32_t size)
{
    Pool_dealloc(tree->pool, node, sizeof(CRbNode) + size);
}

static inline CRbNode*
__RbTree_root(CRbTree* tree)
{
    return (CRbNode*)(tree->header.parAndColor & ~1);
}

static inline void
__RbTree_setRoot(CRbTree* tree, CRbNode* root)
{
    uintptr_t val = (uintptr_t)root | 1;
    tree->header.parAndColor = val;
}

static inline CRbColor
__RbNode_color(CRbNode* node)
{
    return (uintptr_t)node & 1;
}

static inline void
__RbNode_setColor(CRbNode* node, CRbColor color)
{
    uintptr_t val = node->parAndColor;
    if (color == RED)
    {
        val = val & ~1;
    }
    else
    {
        val = val | 1;
    }
    node->parAndColor = val;
}

static void
__RbTree_delete(CRbTree* tree, CRbNode* root, void (*destructor)(void*))
{
    if (root)
    {
        __RbTree_delete(tree, root->left, destructor);
        __RbTree_delete(tree, root->right, destructor);
        destructor(root->attach);
        Pool_dealloc(tree->pool, root, tree->eleSize);
        --tree->count;
    }
}

CRbTree
RbTree_make(int32_t es, CPool* pool)
{
    CRbTree tree = (CRbTree){.pool = pool,
                             .count = 0,
                             .eleSize = es,
                             .header = {0, 0, 0}};
    tree.header.left = tree.header.right = &tree.header;
    return tree;
}

void
RbTree_release(CRbTree* tree, void (*destructor)(void*))
{
    if (tree->count == 0)
    {
        return;
    }
    __RbTree_delete(tree, __RbTree_root(tree), destructor);
    tree->header.parAndColor = 0;
    tree->header.left = &tree->header;
    tree->header.right = &tree->header;
    assert(tree->count == 0);
}

bool
RbTree_empty(CRbTree* tree)
{
    return tree->count == 0 && tree->header.left == &tree->header
           && tree->header.right == &tree->header
           && tree->header.parAndColor == 0;
}
