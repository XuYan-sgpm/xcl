#ifdef STATIC

#  include "xcl/util/rb_tree.h"
#  include <xcl/pool/pool.h>
#  include <gtest/gtest.h>
#  include <xcl/lang/system.h>
#  include <map>
#  include <iostream>
using namespace std;

TEST(RbTreeTest, func1)
{
    cout << sizeof(RbTree) << endl;
    struct RbTree* tree = __RbTree_new(Pool_global());
}

#endif
