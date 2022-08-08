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

TEST(RbTreeTest, func1)
{
    CRbTree tree = RbTree_make(sizeof(int), Pool_def());
    ASSERT_TRUE(RbTree_init(&tree));
    for (int i = 0; i < 1000; i++)
    {
        CRbNode* node = __RbTree_allocNode(&tree);
        *(int*)node->attach = i;
        bool left = true;
        CRbNode* par = __RbTree_findAddPos(&tree,
                                           false,
                                           node->attach,
                                           __cmpInt,
                                           0,
                                           &left);
        ASSERT_TRUE(_RbTree_addNode(&tree, left, par, node));
        ASSERT_TRUE(__RbTree_verify(&tree, __cmpInt, 0));
        cout << "verify successfully" << endl;
    }
    CRbNode* iter = RbTree_begin(&tree);
    int prev = -1;
    for (; iter != RbTree_end(&tree); iter = RbTree_next(&tree, iter))
    {
        cout << *(int*)iter->attach << ' ';
        ASSERT_TRUE(prev < *(int*)iter->attach);
    }
    while (RbTree_empty(&tree) == false)
    {
        _RbTree_rmNode(&tree, RbTree_prev(&tree, iter));
        ASSERT_TRUE(__RbTree_verify(&tree, __cmpInt, 0));
        cout << "verify successfully" << endl;
    }
    cout << endl;
}
