#include <xcl/util/trie_tree.h>
#include <stdlib.h>
#include <stddef.h>

#define VISIBLE_CHARACTER_OFFSET 32
#define VISIBLE_CHARACTER_COUNT  95

typedef struct TrieNode {
    struct TrieNode* children[VISIBLE_CHARACTER_COUNT];
    char data;
    char attach[0];
} TrieNode;

static bool
__TrieNode_isWord(TrieNode* node)
{
    return node->data & 0x80;
}

static void
__TrieNode_setWord(TrieNode* node, bool flag)
{
    if (flag)
        node->data |= 0x80;
    else
        node->data &= 0x7f;
}

static char
__TrieNode_value(TrieNode* node)
{
    return node->data & 0x7f;
}

static void
__TrieNode_setVal(TrieNode* node, char ch)
{
    node->data = (node->data & 0x80) | (ch & 0x7f);
}

static bool
__TrieNode_leaf(TrieNode* node)
{
    for (int i = 0; i < VISIBLE_CHARACTER_COUNT; i++)
    {
        if (node->children[i])
            return false;
    }
    return true;
}

struct TrieTree {
    TrieNode* headers[VISIBLE_CHARACTER_COUNT];
    int32_t count;
    int32_t element_size;
    Pool* pool;
};

static TrieIter
__TrieTree_end(TrieTree* tree)
{
    return (TrieIter){0};
}

static TrieIter
__TrieTree_iter(TrieTree* tree, char ch)
{
    TrieIter ret;
    ret.data[0] = (uintptr_t)tree;
    ret.data[1] = (uintptr_t)(&tree->headers[ch - VISIBLE_CHARACTER_OFFSET]);
    return ret;
}

static TrieIter
__TrieIter_next(TrieIter iter, char ch)
{
    TrieNode* node = *(TrieNode**)iter.data[1];
    TrieIter ret = iter;
    ret.data[1] = (uintptr_t)(&node->children[ch - VISIBLE_CHARACTER_OFFSET]);
    return ret;
}

static TrieNode*
__TrieIter_get(TrieIter iter)
{
    return *(TrieNode**)iter.data[1];
}

static void
__TrieIter_set(TrieIter* iter, TrieNode* node)
{
    *(TrieNode**)iter->data[1] = node;
}

static TrieNode*
__TrieTree_find(TrieTree* tree, const char* word)
{
    if (tree->count == 0)
        return NULL;
    TrieNode* node = tree->headers[word[0] - VISIBLE_CHARACTER_OFFSET];
    while (node)
    {
        ++word;
        if (!word[0])
            break;
        node = node->children[word[0] - VISIBLE_CHARACTER_OFFSET];
    }
    return node;
}

static TrieIter
__TrieTree_find2(TrieTree* tree, const char* word, int32_t* len)
{
    if (len)
        *len = 0;
    const char* p = word;
    TrieIter iter = __TrieTree_iter(tree, p[0]);
    for (;;)
    {
        if (!__TrieIter_get(iter))
            break;
        ++p;
        if (!p[0])
            break;
        iter = __TrieIter_next(iter, p[0]);
    }
    if (len)
        *len = p - word;
    return iter;
}

static void
__TrieTree_clean(TrieTree* tree, TrieNode* top)
{
    for (int i = 0; i < VISIBLE_CHARACTER_COUNT; i++)
    {
        if (top->children[i])
        {
            __TrieTree_clean(tree, top->children[i]);
            top->children[i] = NULL;
        }
    }
    if (__TrieNode_isWord(top))
        --tree->count;
    Pool_dealloc(tree->pool, top, sizeof(*top) + tree->element_size);
}

static TrieNode*
__TrieTree_put(TrieTree* tree, const char* str, TrieIter iter)
{
    if (!str[0])
    {
        TrieNode* node = __TrieIter_get(iter);
        __TrieNode_setWord(node, true);
        ++tree->count;
        return node;
    }
    for (;;)
    {
        TrieNode* new_node = NULL;
        new_node
            = Pool_alloc(tree->pool, sizeof(TrieNode) + tree->element_size);
        if (!new_node)
            return NULL;
        memset(new_node, 0, sizeof(TrieNode) + tree->element_size);
        __TrieNode_setVal(new_node, str[0]);
        __TrieIter_set(&iter, new_node);
        ++str;
        if (!str[0])
        {
            __TrieNode_setWord(new_node, true);
            ++tree->count;
            return new_node;
        }
        iter = __TrieIter_next(iter, str[0]);
    }
}

TrieTree*
TrieTree_new(int32_t size, Pool* pool)
{
    TrieTree* tree = Pool_alloc(pool, sizeof(TrieTree));
    if (!tree)
        return NULL;
    memset(tree, 0, sizeof(*tree));
    tree->element_size = size;
    tree->pool = pool;
    return tree;
}

void
TrieTree_delete(TrieTree* tree)
{
    if (tree->count)
    {
        for (int i = 0; i < VISIBLE_CHARACTER_COUNT; i++)
            __TrieTree_clean(tree, tree->headers[i]);
        memset(tree->headers, 0, sizeof(tree->headers));
    }
    Pool* pool = tree->pool;
    Pool_dealloc(pool, tree, sizeof(*tree));
}

void*
TrieTree_get(TrieTree* tree, const char* word)
{
    TrieNode* node = __TrieTree_find(tree, word);
    if (!node || !tree->element_size)
        return NULL;
    return node->attach;
}

void*
TrieTree_check(TrieTree* tree, const char* word)
{
    if (!word || !word[0])
        return NULL;
    int32_t len = 0;
    TrieIter iter = __TrieTree_find2(tree, word, &len);
    TrieNode* node = __TrieTree_put(tree, word + len, iter);
    if (!node)
        return NULL;
    return node->attach;
}

int32_t
TrieTree_size(TrieTree* tree)
{
    return tree->count;
}

bool
TrieTree_contains(TrieTree* tree, const char* word)
{
    if (!word || word[0])
        return false;
    TrieNode* node = __TrieTree_find(tree, word);
    if (!node || !__TrieNode_isWord(node))
        return false;
    return true;
}

TrieIter
TrieTree_find(TrieTree* tree, const char* word)
{
    if (!word || word[0])
        return __TrieTree_end(tree);
    int32_t len = 0;
    TrieIter iter = __TrieTree_find2(tree, word, &len);
    if (word[len])
        return __TrieTree_end(tree);
    return iter;
}

void*
TrieTree_value(TrieTree* tree, TrieIter iter)
{
    if (tree->element_size == 0)
        return NULL;
    if (tree != (TrieTree*)iter.data[0])
        return NULL;
    TrieNode* node = __TrieIter_get(iter);
    return node && tree->element_size ? node->attach : NULL;
}

bool
TrieTree_remove(TrieTree* tree, const char* word)
{
    int32_t len;
    TrieIter iter = __TrieTree_find2(tree, word, &len);
    if (word[len] || !__TrieNode_isWord(__TrieIter_get(iter)))
        return false;
    TrieTree_removeAt(tree, iter);
    return true;
}

void
TrieTree_removeAt(TrieTree* tree, TrieIter iter)
{
    if ((uintptr_t)tree != iter.data[0])
        return;
    TrieNode* node = __TrieIter_get(iter);
    if (!__TrieNode_isWord(node))
        return;
    if (__TrieNode_leaf(node))
    {
        Pool_dealloc(tree->pool, node, sizeof(TrieNode) + tree->element_size);
        __TrieIter_set(&iter, NULL);
    }
    else
    {
        __TrieNode_setWord(node, false);
    }
    --tree->count;
}

bool
TrieIter_valid(TrieIter iter)
{
    return iter.data[0] && iter.data[1];
}
