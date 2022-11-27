#include <util/rb_tree.h>
#include <xcl/lang/xcl_err.h>
#include <assert.h>
#include <string.h>

#define __RBTREE_ROOT(tree) (RbNode*)((tree)->header.par_and_color & ~1)
#define __RBTREE_SET_ROOT(tree, root)             \
  do                                              \
  {                                               \
    uintptr_t val = (tree)->header.par_and_color; \
    val = (uintptr_t)(root) | (val & 1);          \
    (tree)->header.par_and_color = val;           \
  } while (0);
#define __RBTREE_COLOR(node) (RbColor)((node)->par_and_color & 1)
#define __RBTREE_SET_COLOR(node, color) \
  (node)->par_and_color = ((node)->par_and_color & ~1) | (color)
#define __RBTREE_IS_RED(node)   ((node)->par_and_color & 1) == 0
#define __RBTREE_IS_BLACK(node) ((node)->par_and_color & 1) == 1
#define __RBTREE_SET_PARENT(node, par) \
  (node)->par_and_color = (uintptr_t)(par) | ((node)->par_and_color & 1)
#define __RBTREE_PARENT(node) (RbNode*)((node)->par_and_color & ~1)
#define __RBTREE_ATTACH(node) (void*)((node) + 1)

RbNode*
__RbTree_allocNode(RbTree* tree, int32_t element_size)
{
    void* ptr = Pool_alloc(tree->pool, sizeof(RbNode) + element_size);
    if (!ptr)
        return NULL;
    if (((uintptr_t)ptr & 1) == 0)
    {
        RbNode* node = ptr;
        node->left = node->right = NULL;
        node->par_and_color = 0;
        return ptr;
    }
    Pool_dealloc(tree->pool, ptr, sizeof(RbNode) + element_size);
    return NULL;
}

void
__RbTree_deallocNode(RbTree* tree, RbNode* node, int32_t element_size)
{
    Pool_dealloc(tree->pool, node, sizeof(RbNode) + element_size);
}

RbNode*
__RbTree_root(RbTree* tree)
{
    return __RBTREE_ROOT(tree);
}

RbNode*
__RbNode_min(RbNode* root);

RbNode*
__RbNode_max(RbNode* root);

bool
__RbTree_addNode(RbTree* tree, bool left, RbNode* par, RbNode* new_node);

RbNode*
__RbTree_removeNode(RbTree* tree, RbNode* node);

void
__RbTree_init(RbTree* tree, Pool* pool)
{
    memset(tree, 0, sizeof(RbTree));
    RbNode* header = (&tree->header);
    header->left = header->right = header;
    header->par_and_color = 0;
    *(Pool**)&tree->pool = pool;
}

RbTree*
__RbTree_new(Pool* pool)
{
    RbTree* tree = Pool_alloc(pool, sizeof(RbTree));
    if (!tree)
        return NULL;
    __RbTree_init(tree, pool);
    return tree;
}

bool
__RbTree_empty(RbTree* tree)
{
    return tree->count == 0 && (&tree->header)->left == (&tree->header) &&
           (&tree->header)->right == (&tree->header) &&
           (&tree->header)->par_and_color == 0;
}

static int32_t
__RbTree_blackNodesUp(RbTree* tree, RbNode* node)
{
    int32_t count = 0;
    while (node != (&tree->header))
    {
        if (__RBTREE_IS_BLACK(node))
        {
            ++count;
        }
        node = __RBTREE_PARENT(node);
    }
    return count;
}

void
__RbNode_swapColor(RbNode* node1, RbNode* node2)
{
    RbColor color = __RBTREE_COLOR(node1);
    __RBTREE_SET_COLOR(node1, __RBTREE_COLOR(node2));
    __RBTREE_SET_COLOR(node2, color);
}

RbNode*
__RbNode_min(RbNode* root)
{
    RbNode* minimum = root;
    if (minimum)
    {
        while (minimum->left)
        {
            minimum = minimum->left;
        }
    }
    return minimum;
}

RbNode*
__RbNode_max(RbNode* root)
{
    RbNode* maximum = root;
    if (maximum)
    {
        while (maximum->right)
        {
            maximum = maximum->right;
        }
    }
    return maximum;
}

void
__RbTree_leftRotate(RbTree* tree, RbNode* node)
{
    RbNode* r = node->right;
    RbNode* rl = r->left;
    node->right = rl;
    if (rl)
    {
        __RBTREE_SET_PARENT(rl, node);
    }
    RbNode* par = __RBTREE_PARENT(node);
    if (node == __RBTREE_ROOT(tree))
    {
        __RBTREE_SET_ROOT(tree, r);
    }
    else if (node == par->left)
    {
        par->left = r;
    }
    else
    {
        par->right = r;
    }
    __RBTREE_SET_PARENT(r, par);
    r->left = node;
    __RBTREE_SET_PARENT(node, r);
}

void
__RbTree_rightRotate(RbTree* tree, RbNode* node)
{
    RbNode* l = node->left;
    RbNode* lr = l->right;
    node->left = lr;
    if (lr)
    {
        __RBTREE_SET_PARENT(lr, node);
    }
    RbNode* par = __RBTREE_PARENT(node);
    if (par == (&tree->header))
    {
        __RBTREE_SET_ROOT(tree, l);
    }
    else if (node == par->left)
    {
        par->left = l;
    }
    else
    {
        par->right = l;
    }
    __RBTREE_SET_PARENT(l, par);
    l->right = node;
    __RBTREE_SET_PARENT(node, l);
}

void
__RbTree_insertRebalance(RbTree* tree, RbNode* node)
{
    __RBTREE_SET_COLOR(node, RED);
    RbNode* root = __RBTREE_ROOT(tree);
    RbNode* par;
    while (node != root && __RBTREE_IS_RED((par = __RBTREE_PARENT(node))))
    {
        RbNode* grand = __RBTREE_PARENT(par);
        if (par == grand->left)
        {
            RbNode* uncle = grand->right;
            if (uncle && __RBTREE_IS_RED(uncle))
            {
                __RBTREE_SET_COLOR(par, BLACK);
                __RBTREE_SET_COLOR(uncle, BLACK);
                __RBTREE_SET_COLOR(grand, RED);
                node = grand;
            }
            else
            {
                if (node == par->right)
                {
                    __RbTree_leftRotate(tree, par);
                    node = par;
                    par = __RBTREE_PARENT(node);
                }
                __RBTREE_SET_COLOR(par, BLACK);
                __RBTREE_SET_COLOR(grand, RED);
                __RbTree_rightRotate(tree, grand);
                break;
            }
        }
        else
        {
            RbNode* uncle = grand->left;
            if (uncle && __RBTREE_IS_RED(uncle))
            {
                __RBTREE_SET_COLOR(par, BLACK);
                __RBTREE_SET_COLOR(uncle, BLACK);
                __RBTREE_SET_COLOR(grand, RED);
                node = grand;
            }
            else
            {
                if (node == par->left)
                {
                    __RbTree_rightRotate(tree, par);
                    node = par;
                    par = __RBTREE_PARENT(node);
                }
                __RBTREE_SET_COLOR(par, BLACK);
                __RBTREE_SET_COLOR(grand, RED);
                __RbTree_leftRotate(tree, grand);
                break;
            }
        }
    }
    __RBTREE_SET_COLOR(__RBTREE_ROOT(tree), BLACK);
}

void
__RbTree_removeRebalance(RbTree* tree, RbNode* par, RbNode* node)
{
    RbNode* cousin;
    RbNode* root = __RBTREE_ROOT(tree);
    while (node != root && (!node || __RBTREE_IS_BLACK(node)))
    {
        if (node == par->left)
        {
            cousin = par->right;
            if (__RBTREE_IS_RED(cousin))
            {
                __RBTREE_SET_COLOR(cousin, BLACK);
                __RBTREE_SET_COLOR(par, RED);
                __RbTree_leftRotate(tree, par);
                cousin = par->right;
            }
            if ((!cousin->left || __RBTREE_IS_BLACK(cousin->left)) &&
                (!cousin->right || __RBTREE_IS_BLACK(cousin->right)))
            {
                __RBTREE_SET_COLOR(cousin, RED);
                node = par;
                par = __RBTREE_PARENT(node);
            }
            else
            {
                if (!cousin->right || __RBTREE_IS_BLACK(cousin->right))
                {
                    __RBTREE_SET_COLOR(cousin, RED);
                    __RBTREE_SET_COLOR(cousin->left, BLACK);
                    __RbTree_rightRotate(tree, cousin);
                    cousin = par->right;
                }
                __RBTREE_SET_COLOR(cousin, __RBTREE_COLOR(par));
                __RBTREE_SET_COLOR(par, BLACK);
                __RBTREE_SET_COLOR(cousin->right, BLACK);
                __RbTree_leftRotate(tree, par);
                break;
            }
        }
        else
        {
            cousin = par->left;
            if (__RBTREE_IS_RED(cousin))
            {
                __RBTREE_SET_COLOR(cousin, BLACK);
                __RBTREE_SET_COLOR(par, RED);
                __RbTree_rightRotate(tree, par);
                cousin = par->left;
            }
            if ((!cousin->left || __RBTREE_IS_BLACK(cousin->left)) &&
                (!cousin->right || __RBTREE_IS_BLACK(cousin->right)))
            {
                __RBTREE_SET_COLOR(cousin, RED);
                node = par;
                par = __RBTREE_PARENT(node);
            }
            else
            {
                if (!cousin->left || __RBTREE_IS_BLACK(cousin->left))
                {
                    __RBTREE_SET_COLOR(cousin, RED);
                    __RBTREE_SET_COLOR(cousin->right, BLACK);
                    __RbTree_leftRotate(tree, cousin);
                    cousin = par->left;
                }
                __RBTREE_SET_COLOR(cousin, __RBTREE_COLOR(par));
                __RBTREE_SET_COLOR(cousin->left, BLACK);
                __RBTREE_SET_COLOR(par, BLACK);
                __RbTree_rightRotate(tree, par);
                break;
            }
        }
    }
    if (node)
    {
        __RBTREE_SET_COLOR(node, BLACK);
    }
}

bool
__RbTree_addNode(RbTree* tree, bool left, RbNode* par, RbNode* new_node)
{
    __RBTREE_SET_PARENT(new_node, par);
    if (par == (&tree->header))
    {
        assert(__RbTree_empty(tree));
        __RBTREE_SET_ROOT(tree, new_node);
        (&tree->header)->left = new_node;
        (&tree->header)->right = new_node;
        __RBTREE_SET_COLOR(new_node, BLACK);
    }
    else
    {
        if (left)
        {
            par->left = new_node;
            if (par == (&tree->header)->left)
            {
                (&tree->header)->left = new_node;
            }
        }
        else
        {
            par->right = new_node;
            if (par == (&tree->header)->right)
            {
                (&tree->header)->right = new_node;
            }
        }
        __RbTree_insertRebalance(tree, new_node);
    }
    tree->count += 1;
    return true;
}

RbNode*
__RbTree_removeNode(RbTree* tree, RbNode* node)
{
    RbNode* suc = node; // node replace at position of removed node
    RbNode* suc2 = 0;   // node replace at position of suc
    RbNode* suc2_par = 0;
    if (!node->left)
    {
        suc2 = node->right;
    }
    else if (!node->right)
    {
        suc2 = node->left;
    }
    else
    {
        suc = __RbNode_max(node->left);
        suc2 = suc->left;
    }
    RbNode* par = __RBTREE_PARENT(node);
    if (suc != node)
    {
        if (suc == node->left)
        {
            suc2_par = suc;
        }
        else
        {
            suc2_par = __RBTREE_PARENT(suc);
            suc2_par->right = suc2;
            if (suc2)
            {
                __RBTREE_SET_PARENT(suc2, suc2_par);
            }
            suc->left = node->left;
            __RBTREE_SET_PARENT(node->left, suc);
        }
        suc->right = node->right;
        __RBTREE_SET_PARENT(node->right, suc);
        if (__RBTREE_ROOT(tree) == node)
        {
            __RBTREE_SET_ROOT(tree, suc);
        }
        else if (par->left == node)
        {
            par->left = suc;
        }
        else
        {
            par->right = suc;
        }
        __RBTREE_SET_PARENT(suc, par);
        __RbNode_swapColor(suc, node);
    }
    else
    {
        suc2_par = par;
        if (__RBTREE_ROOT(tree) == node)
        {
            __RBTREE_SET_ROOT(tree, suc2);
        }
        else if (par->left == node)
        {
            par->left = suc2;
        }
        else
        {
            par->right = suc2;
        }
        if (suc2)
        {
            __RBTREE_SET_PARENT(suc2, suc2_par);
        }
        if (node == (&tree->header)->left)
        {
            (&tree->header)->left = !suc2 ? par : __RbNode_min(suc2);
        }
        if (node == (&tree->header)->right)
        {
            (&tree->header)->right = !suc2 ? par : __RbNode_max(suc2);
        }
    }
    if (__RBTREE_IS_BLACK(node))
    {
        __RbTree_removeRebalance(tree, suc2_par, suc2);
    }
    --tree->count;
    return node;
}

RbNode*
__RbTree_begin(RbTree* tree)
{
    return (&tree->header)->left;
}

RbNode*
__RbTree_end(RbTree* tree)
{
    return (&tree->header);
}

RbNode*
__RbTree_next(RbTree* tree, RbNode* node)
{
    if (node == (&tree->header))
    {
        return node;
    }
    if (node->right)
    {
        return __RbNode_min(node->right);
    }
    RbNode* par;
    while ((par = __RBTREE_PARENT(node)) != (&tree->header) &&
           par->right == node)
    {
        node = par;
    }
    return par;
}

RbNode*
__RbTree_prev(RbTree* tree, RbNode* node)
{
    if (node == (&tree->header))
    {
        return (&tree->header)->right;
    }
    if (node == (&tree->header)->left)
    {
        return node;
    }
    if (node->left)
    {
        return __RbNode_max(node->left);
    }
    RbNode* par;
    while ((par = __RBTREE_PARENT(node))->left == node)
    {
        node = par;
    }
    return par;
}
