#include "xcl/util/CRbTree.h"
#include <xcl/lang/XclErr.h>
#include <assert.h>
#include <string.h>

CRbNode*
__RbTree_allocNode(CRbTree* tree)
{
    void* p = Pool_alloc(tree->pool, sizeof(CRbNode) + tree->eleSize);
    if (p)
    {
        assert(((uintptr_t)p & 1) == 0);
        CRbNode* node = p;
        node->left = node->right = 0;
        node->parAndColor = 0;
    }
    return p;
}

void
__RbTree_deallocNode(CRbTree* tree, CRbNode* node)
{
    Pool_dealloc(tree->pool, node, sizeof(CRbNode) + tree->eleSize);
}

static inline CRbNode*
__RbTree_root(CRbTree* tree)
{
    return (CRbNode*)(tree->header.parAndColor & ~1);
}

static inline void
__RbTree_setRoot(CRbTree* tree, CRbNode* root)
{
    uintptr_t val = tree->header.parAndColor;
    val = (uintptr_t)root | (val & 1);
    tree->header.parAndColor = val;
}

static inline CRbColor
__RbNode_color(CRbNode* node)
{
    return node->parAndColor & 1;
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

static inline void
__RbNode_swapColor(CRbNode* node1, CRbNode* node2)
{
    CRbColor color = __RbNode_color(node1);
    __RbNode_setColor(node1, __RbNode_color(node2));
    __RbNode_setColor(node2, color);
}

static inline bool
__RbNode_isRed(CRbNode* node)
{
    return (node->parAndColor & 1) == 0;
}

static inline bool
__RbNode_isBlack(CRbNode* node)
{
    return (node->parAndColor & 1) == 1;
}

static inline void
__RbNode_setPar(CRbNode* node, CRbNode* par)
{
    node->parAndColor = (uintptr_t)par | (node->parAndColor & 1);
}

static inline CRbNode*
__RbNode_par(CRbNode* node)
{
    return (CRbNode*)(node->parAndColor & ~1);
}

static CRbNode*
__RbNode_min(CRbNode* root)
{
    CRbNode* minimum = root;
    if (minimum)
    {
        while (minimum->left)
        {
            minimum = minimum->left;
        }
    }
    return minimum;
}

static CRbNode*
__RbNode_max(CRbNode* root)
{
    CRbNode* maximum = root;
    if (maximum)
    {
        while (maximum->right)
        {
            maximum = maximum->right;
        }
    }
    return maximum;
}

static void
__RbTree_leftRotate(CRbTree* tree, CRbNode* node)
{
    CRbNode* r = node->right;
    CRbNode* rl = r->left;
    node->right = rl;
    if (rl)
    {
        __RbNode_setPar(rl, node);
    }
    CRbNode* par = __RbNode_par(node);
    if (node == __RbTree_root(tree))
    {
        __RbTree_setRoot(tree, r);
    }
    else
    {
        if (node == par->left)
        {
            par->left = r;
        }
        else
        {
            par->right = r;
        }
    }
    __RbNode_setPar(r, par);
    r->left = node;
    __RbNode_setPar(node, r);
}

static void
__RbTree_rightRotate(CRbTree* tree, CRbNode* node)
{
    CRbNode* l = node->left;
    CRbNode* lr = l->right;
    node->left = lr;
    if (lr)
    {
        __RbNode_setPar(lr, node);
    }
    CRbNode* par = __RbNode_par(node);
    if (par == &tree->header)
    {
        __RbTree_setRoot(tree, l);
    }
    else if (node == par->left)
    {
        par->left = l;
    }
    else
    {
        par->right = l;
    }
    __RbNode_setPar(l, par);
    l->right = node;
    __RbNode_setPar(node, l);
}

static void
__RbTree_insertRebalance(CRbTree* tree, CRbNode* node)
{
    __RbNode_setColor(node, RED);
    CRbNode* root = __RbTree_root(tree);
    CRbNode* par;
    while (node != root && __RbNode_isRed((par = __RbNode_par(node))))
    {
        CRbNode* grand = __RbNode_par(par);
        if (par == grand->left)
        {
            CRbNode* uncle = grand->right;
            if (uncle && __RbNode_isRed(uncle))
            {
                __RbNode_setColor(par, BLACK);
                __RbNode_setColor(uncle, BLACK);
                __RbNode_setColor(grand, RED);
                node = grand;
            }
            else
            {
                if (node == par->right)
                {
                    __RbTree_leftRotate(tree, par);
                    node = par;
                    par = __RbNode_par(node);
                }
                __RbNode_setColor(par, BLACK);
                __RbNode_setColor(grand, RED);
                __RbTree_rightRotate(tree, grand);
                break;
            }
        }
        else
        {
            CRbNode* uncle = grand->left;
            if (uncle && __RbNode_isRed(uncle))
            {
                __RbNode_setColor(par, BLACK);
                __RbNode_setColor(uncle, BLACK);
                __RbNode_setColor(grand, RED);
                node = grand;
            }
            else
            {
                if (node == par->left)
                {
                    __RbTree_rightRotate(tree, par);
                    node = par;
                    par = __RbNode_par(node);
                }
                __RbNode_setColor(par, BLACK);
                __RbNode_setColor(grand, RED);
                __RbTree_leftRotate(tree, grand);
                break;
            }
        }
    }
    __RbNode_setColor(__RbTree_root(tree), BLACK);
}

static void
__RbTree_delRebalance(CRbTree* tree, CRbNode* par, CRbNode* node)
{
    CRbNode* cousin;
    CRbNode* root = __RbTree_root(tree);
    while (node != root && (!node || __RbNode_isBlack(node)))
    {
        if (node == par->left)
        {
            cousin = par->right;
            if (__RbNode_isRed(cousin))
            {
                __RbNode_setColor(cousin, __RbNode_color(par));
                __RbNode_setColor(par, BLACK);
                __RbTree_leftRotate(tree, par);
                cousin = par->right;
            }
            if ((!cousin->left || __RbNode_isBlack(cousin->left))
                && (!cousin->right || __RbNode_isBlack(cousin->right)))
            {
                __RbNode_setColor(cousin, RED);
                node = par;
                par = __RbNode_par(node);
            }
            else
            {
                if (!cousin->right || __RbNode_isBlack(cousin->right))
                {
                    __RbNode_setColor(cousin, RED);
                    __RbNode_setColor(cousin->left, BLACK);
                    __RbTree_rightRotate(tree, cousin);
                    cousin = par->right;
                }
                __RbNode_setColor(cousin, __RbNode_color(par));
                __RbNode_setColor(par, BLACK);
                __RbNode_setColor(cousin->right, BLACK);
                __RbTree_leftRotate(tree, par);
                break;
            }
        }
        else
        {
            cousin = par->left;
            if (__RbNode_isRed(cousin))
            {
                __RbNode_setColor(cousin, BLACK);
                __RbNode_setColor(par, RED);
                __RbTree_rightRotate(tree, par);
                cousin = par->left;
            }
            if ((!cousin->left || __RbNode_isBlack(cousin->left))
                && (!cousin->right || __RbNode_isBlack(cousin->right)))
            {
                __RbNode_setColor(cousin, RED);
                node = par;
                par = __RbNode_par(node);
            }
            else
            {
                if (!cousin->left || __RbNode_isBlack(cousin->left))
                {
                    __RbNode_setColor(cousin, RED);
                    __RbNode_setColor(cousin->right, BLACK);
                    __RbTree_leftRotate(tree, cousin);
                    cousin = par->left;
                }
                __RbNode_setColor(cousin, __RbNode_color(par));
                __RbNode_setColor(cousin->left, BLACK);
                __RbNode_setColor(par, BLACK);
                __RbTree_rightRotate(tree, par);
                break;
            }
        }
    }
    if (node)
    {
        __RbNode_setColor(node, BLACK);
    }
}

bool
_RbTree_addNode(CRbTree* tree, bool left, CRbNode* par, CRbNode* newNode)
{
    __RbNode_setPar(newNode, par);
    if (par == &tree->header)
    {
        assert(RbTree_empty(tree));
        __RbTree_setRoot(tree, newNode);
        tree->header.left = newNode;
        tree->header.right = newNode;
        __RbNode_setColor(newNode, BLACK);
    }
    else
    {
        if (left)
        {
            par->left = newNode;
            if (par == tree->header.left)
            {
                tree->header.left = newNode;
            }
        }
        else
        {
            par->right = newNode;
            if (par == tree->header.right)
            {
                tree->header.right = newNode;
            }
        }
        __RbTree_insertRebalance(tree, newNode);
    }
    tree->count += 1;
    return true;
}

CRbNode*
_RbTree_rmNode(CRbTree* tree, CRbNode* node)
{
    CRbNode* suc = node; // node replace at position of node
    CRbNode* suc2 = 0;   // node replace at position of suc
    CRbNode* suc2Par = 0;
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
    CRbNode* par = __RbNode_par(node);
    if (suc != node)
    {
        if (suc == node->left)
        {
            suc2 = suc;
            suc2Par = par;
        }
        else
        {
            suc2Par = __RbNode_par(suc);
            suc2Par->right = suc2;
            if (suc2)
            {
                __RbNode_setPar(suc2, suc2Par);
            }
            suc->left = node->left;
            __RbNode_setPar(node->left, suc);
        }
        suc->right = node->right;
        __RbNode_setPar(node->right, suc);
        if (__RbTree_root(tree) == node)
        {
            __RbTree_setRoot(tree, suc);
        }
        else if (par->left == node)
        {
            par->left = suc;
        }
        else
        {
            par->right = suc;
        }
        __RbNode_setPar(suc, par);
        __RbNode_swapColor(suc, node);
    }
    else
    {
        suc2Par = par;
        if (__RbTree_root(tree) == node)
        {
            __RbTree_setRoot(tree, suc2);
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
            __RbNode_setPar(suc2, suc2Par);
        }
        if (node == tree->header.left)
        {
            tree->header.left = !suc2 ? par : __RbNode_min(suc2);
        }
        if (node == tree->header.right)
        {
            tree->header.right = !suc2 ? par : __RbNode_max(suc2);
        }
    }
    if (__RbNode_isBlack(node))
    {
        __RbTree_delRebalance(tree, suc2Par, suc2);
    }
    --tree->count;
    return node;
}

CRbNode*
_RbTree_find(CRbTree* tree,
             const void* src,
             int32_t (*cmp)(void*, const void*, const void*),
             void* usr)
{
    CRbNode* found = __RbTree_root(tree);
    while (found)
    {
        int32_t ret = cmp(usr, src, found->attach);
        if (ret == 0)
        {
            return found;
        }
        if (ret < 0)
        {
            found = found->left;
        }
        else
        {
            found = found->right;
        }
    }
    return found;
}

CRbNode*
__RbTree_findAddPos(CRbTree* tree,
                    bool interruptIfFound,
                    const void* src,
                    int32_t (*cmp)(void*, const void*, const void*),
                    void* usr,
                    bool* left)
{
    CRbNode* par = &tree->header;
    CRbNode* cur = __RbTree_root(tree);
    while (cur)
    {
        par = cur;
        int32_t ret = cmp(usr, src, cur->attach);
        if (ret <= 0)
        {
            if (ret == 0 && interruptIfFound)
            {
                return NULL;
            }
            *left = true;
            cur = cur->left;
        }
        else
        {
            *left = false;
            cur = cur->right;
        }
    }
    return par;
}

static void
__RbTree_delete(CRbTree* tree,
                CRbNode* root,
                void (*destructor)(void*, void*),
                void* usr)
{
    if (root)
    {
        __RbTree_delete(tree, root->left, destructor, usr);
        __RbTree_delete(tree, root->right, destructor, usr);
        destructor(usr, root->attach);
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
RbTree_release(CRbTree* tree, void (*destructor)(void*, void*), void* usr)
{
    if (tree->count == 0)
    {
        return;
    }
    __RbTree_delete(tree, __RbTree_root(tree), destructor, usr);
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

static CRbNode*
__RbTree_lowerBound(CRbTree* tree,
                    const void* src,
                    int32_t (*cmp)(void*, const void*, const void*),
                    void* usr)
{
    CRbNode* cur = __RbTree_root(tree);
    CRbNode* lower = &tree->header;
    while (cur)
    {
        int32_t ret = cmp(usr, src, cur->attach);
        if (ret >= 0)
        {
            lower = cur;
            cur = cur->right;
        }
        else
        {
            cur = cur->left;
        }
    }
    return lower;
}

static CRbNode*
__RbTree_upperBound(CRbTree* tree,
                    const void* src,
                    int32_t (*cmp)(void*, const void*, const void*),
                    void* usr)
{
    CRbNode* cur = __RbTree_root(tree);
    CRbNode* upper = &tree->header;
    while (cur)
    {
        int32_t ret = cmp(usr, src, cur->attach);
        if (ret < 0)
        {
            upper = cur;
            cur = cur->left;
        }
        else
        {
            cur = cur->right;
        }
    }
    return upper;
}

void
RbTree_range(CRbTree* tree,
             const void* src,
             int32_t (*cmp)(void*, const void*, const void*),
             void* usr,
             CRbNode** lower,
             CRbNode** upper)
{
    if (lower)
        *lower = __RbTree_lowerBound(tree, src, cmp, usr);
    if (upper)
        *upper = __RbTree_upperBound(tree, src, cmp, usr);
}

CRbNode*
RbTree_begin(CRbTree* tree)
{
    return tree->header.left;
}

CRbNode*
RbTree_end(CRbTree* tree)
{
    return &tree->header;
}

CRbNode*
RbTree_next(CRbTree* tree, CRbNode* node)
{
    if (node == &tree->header)
    {
        return node;
    }
    if (node->right)
    {
        return __RbNode_min(node->right);
    }
    CRbNode* par;
    while ((par = __RbNode_par(node))->right == node && par != &tree->header)
    {
        node = par;
    }
    return par;
}

CRbNode*
RbTree_prev(CRbTree* tree, CRbNode* node)
{
    if (node == &tree->header)
    {
        return tree->header.right;
    }
    if (node == tree->header.left)
    {
        return node;
    }
    if (node->left)
    {
        return __RbNode_max(node->left);
    }
    CRbNode* par;
    while ((par = __RbNode_par(node))->left == node)
    {
        node = par;
    }
    return par;
}

static CRbNode*
__RbTree_makeSub(CRbTree* tree,
                 CRbNode* par,
                 CRbNode* top,
                 bool (*constructor)(void*, void*, const void*),
                 void* usr,
                 bool* complete)
{
    *complete = false;
    if (!top)
    {
        return 0;
    }
    CRbNode* newNode = __RbTree_allocNode(tree);
    if (!newNode)
    {
        return 0;
    }
    newNode->left = newNode->right = 0;
    __RbNode_setColor(newNode, __RbNode_color(top));
    if (constructor(usr, newNode->attach, top->attach))
    {
        tree->count += 1;
        __RbNode_setPar(newNode, par);
        if (top->left)
        {
            bool leftComplete = false;
            newNode->left = __RbTree_makeSub(tree,
                                             newNode,
                                             top->left,
                                             constructor,
                                             usr,
                                             &leftComplete);
            if (!leftComplete)
            {
                return newNode;
            }
        }
        if (top->right)
        {
            bool rightComplete = false;
            newNode->right = __RbTree_makeSub(tree,
                                              newNode,
                                              top->right,
                                              constructor,
                                              usr,
                                              &rightComplete);
            if (!rightComplete)
            {
                return newNode;
            }
        }
        *complete = true;
        return newNode;
    }
    __RbTree_deallocNode(tree, newNode);
    return 0;
}

bool
__RbTree_copy(CRbTree* tree,
              bool (*constructor)(void*, void*, const void*),
              void* usr,
              CRbTree* dst)
{
    if (!RbTree_empty(dst))
    {
        return false;
    }
    bool complete;
    CRbNode* copy = __RbTree_makeSub(dst,
                                     &dst->header,
                                     __RbTree_root(tree),
                                     constructor,
                                     usr,
                                     &complete);
    if (copy)
    {
        __RbTree_setRoot(dst, copy);
        dst->header.left = __RbNode_min(copy);
        dst->header.right = __RbNode_max(copy);
        return complete;
    }
    return false;
}

static int32_t
__RbTree_blackNodesUp(CRbTree* tree, CRbNode* node)
{
    int32_t count = 0;
    while (node != &tree->header)
    {
        if (__RbNode_isBlack(node))
        {
            ++count;
        }
        node = __RbNode_par(node);
    }
    return count;
}

bool
__RbTree_verify(CRbTree* tree,
                int32_t (*cmp)(void*, const void*, const void*),
                void* usr)
{
    if (tree->count == 0)
    {
        if (tree->header.left == &tree->header
            && tree->header.right == &tree->header
            && tree->header.parAndColor == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    int32_t blacks = __RbTree_blackNodesUp(tree, tree->header.left);
    if (blacks != __RbTree_blackNodesUp(tree, tree->header.right))
    {
        return false;
    }
    int traverse = 0;
    CRbNode* cur = RbTree_begin(tree);
    for (; cur != &tree->header; cur = RbTree_next(tree, cur))
    {
        ++traverse;
        CRbNode* left = cur->left;
        CRbNode* right = cur->right;
        if (__RbNode_isRed(cur))
        {
            if (left && __RbNode_isRed(left))
            {
                return false;
            }
            if (right && __RbNode_isRed(right))
            {
                return false;
            }
        }
        if (left && cmp(usr, left->attach, cur->attach) > 0)
        {
            return false;
        }
        if (right && cmp(usr, right->attach, cur->attach) < 0)
        {
            return false;
        }
        if (!left && !right)
        {
            if (blacks != __RbTree_blackNodesUp(tree, cur))
            {
                return false;
            }
        }
    }
    if (traverse != tree->count)
    {
        return false;
    }
    if (tree->header.left != __RbNode_min(__RbTree_root(tree)))
    {
        return false;
    }
    if (tree->header.right != __RbNode_max(__RbTree_root(tree)))
    {
        return false;
    }
    return true;
}
