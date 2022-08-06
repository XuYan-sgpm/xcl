#include "xcl/util/CRbTree.h"
#include <xcl/pool/CPool.h>
#include <gtest/gtest.h>

TEST(RbTreeTest,func1){
    CRbTree tree = RbTree_make(sizeof(int), Pool_def());
}
