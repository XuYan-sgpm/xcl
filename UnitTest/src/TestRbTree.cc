#include "xcl/util/CRbTree.h"
#include <xcl/pool/CPool.h>
#include <gtest/gtest.h>
#include <iostream>
using namespace std;

static int
__cmpInt(void*, const void* lhs, const void* rhs)
{
    return *(int*)lhs - *(int*)rhs;
}

static bool
__setInt(void*, void* lhs, const void* rhs)
{
    *(int*)lhs = *(int*)rhs;
    return true;
}

static bool
__destroy(void*, void*)
{
    return true;
}

TEST(RbTreeTest, func1)
{
    CRbTree tree = RbTree_make(sizeof(int), Pool_def());
    ASSERT_TRUE(RbTree_init(&tree));
    for (int i = 0; i < 1000; i++)
    {
        CRbNode* node = __RbTree_allocNode(&tree);
        *(int*)node->attach = rand();
        cout << *(int*)node->attach << ',';
        bool left = true;
        CRbNode* par = __RbTree_findAddPos(&tree,
                                           false,
                                           node->attach,
                                           __cmpInt,
                                           0,
                                           &left);
        ASSERT_TRUE(_RbTree_addNode(&tree, left, par, node));
        ASSERT_TRUE(__RbTree_verify(&tree, __cmpInt, 0));
    }
    cout << endl << endl;
    CRbNode* iter = RbTree_begin(&tree);
    int prev = -1;
    for (; iter != RbTree_end(&tree); iter = RbTree_next(&tree, iter))
    {
        cout << *(int*)iter->attach << ' ';
        ASSERT_TRUE(prev < *(int*)iter->attach);
    }
    cout << endl << endl;
    prev = -1;
    CRbNode* del = 0;
    while (RbTree_empty(&tree) == false)
    {
        iter = _RbTree_rmNode(&tree, RbTree_prev(&tree, iter));
        __RbTree_deallocNode(&tree, del);
        del = iter;
        if (tree.count == 999)
        {
            prev = *(int*)iter->attach;
        }
        else
        {
            ASSERT_TRUE(*(int*)iter->attach <= prev);
            prev = *(int*)iter->attach;
        }
        cout << prev << ",";
        ASSERT_TRUE(__RbTree_verify(&tree, __cmpInt, 0));
    }
    __RbTree_deallocNode(&tree, del);
    cout << endl;
}

TEST(RbTreeTest, func2)
{
    auto tree = RbTree_make(sizeof(int), Pool_def());
    ASSERT_TRUE(RbTree_init(&tree));
    for (int i = 0; i < 1024; i++)
    {
        int val = rand();
        auto node = __RbTree_allocNode(&tree);
        *(int*)node->attach = val;
        bool left;
        auto par = __RbTree_findAddPos(&tree,
                                       false,
                                       node->attach,
                                       __cmpInt,
                                       0,
                                       &left);
        ASSERT_TRUE(_RbTree_addNode(&tree, left, par, node));
        ASSERT_TRUE(__RbTree_verify(&tree, __cmpInt, 0));
    }
    auto copy = RbTree_make(sizeof(int), Pool_def());
    ASSERT_TRUE(RbTree_init(&copy));
    ASSERT_TRUE(__RbTree_copy(&tree, __setInt, 0, &copy));
    auto iter1 = RbTree_begin(&tree);
    auto iter2 = RbTree_begin(&copy);
    ASSERT_EQ(tree.count, copy.count);
    for (; iter1 != RbTree_end(&tree);
         iter1 = RbTree_next(&tree, iter1), iter2 = RbTree_next(&copy, iter2))
    {
        ASSERT_EQ(*(int*)iter1->attach, *(int*)iter2->attach);
    }
}
