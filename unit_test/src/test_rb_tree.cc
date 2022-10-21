// #ifdef STATIC
// #include "xcl/util/rb_tree.h"
// #include <xcl/pool/pool.h>
// #include <gtest/gtest.h>
// #include <xcl/lang/system.h>
// #include <map>
// #include <iostream>
// using namespace std;

// TEST(RbTreeTest, func1)
// {
//     cout << sizeof(RbTree) << endl;
//     struct RbTree* tree = __RbTree_new(Pool_def());
//     // ASSERT_TRUE(__RbTree_init(tree));
//     for (int i = 0; i < 1000; i++)
//     {
//         int data = rand();
//         ASSERT_TRUE(__RbTree_add(tree, &data, nullptr));
//         ASSERT_TRUE(__RbTree_verify(tree));
//     }
//     cout << endl << endl;
//     struct RbNode* iter = __RbTree_begin(tree);
//     int prev = -1;
//     for (; iter != __RbTree_end(tree); iter = __RbTree_next(tree, iter))
//     {
//         cout << *(int*)(iter + 1) << ' ';
//         ASSERT_TRUE(prev < *(int*)(iter + 1));
//     }
//     cout << endl << endl;
//     prev = -1;
//     iter = __RbTree_prev(tree, iter);
//     while (__RbTree_empty(tree) == false)
//     {
//         RbNode* iterPrev = __RbTree_prev(tree, iter);
//         // __RbTree_deallocNode(tree, iter);
//         // del = iter;
//         if (tree->count == 1000)
//         {
//             prev = *(int*)(iter + 1);
//         }
//         else
//         {
//             ASSERT_TRUE(*(int*)(iter + 1) <= prev);
//             prev = *(int*)(iter + 1);
//         }
//         __RbTree_deleteNode(tree, iter);
//         iter = iterPrev;
//         cout << prev << ",";
//         ASSERT_TRUE(__RbTree_verify(tree));
//     }
//     // __RbTree_deallocNode(tree, del);
//     cout << endl;
// }

// TEST(RbTreeTest, func2)
// {
//     auto tree = __RbTree_new(Pool_def());
//     // ASSERT_TRUE(__RbTree_init(tree));
//     for (int i = 0; i < 1024; i++)
//     {
//         int val = rand();
//         ASSERT_TRUE(__RbTree_add(tree, &val, __setInt));
//         ASSERT_TRUE(__RbTree_verify(tree));
//     }
//     auto copy = __RbTree_new(Pool_def());
//     // ASSERT_TRUE(__RbTree_init(copy));
//     ASSERT_TRUE(__RbTree_copy(tree, __setInt, nullptr, copy));
//     auto iter1 = __RbTree_begin(tree);
//     auto iter2 = __RbTree_begin(copy);
//     ASSERT_EQ(tree->count, copy->count);
//     for (; iter1 != __RbTree_end(tree);
//          iter1 = __RbTree_next(tree, iter1), iter2 = __RbTree_next(copy,
//          iter2))
//     {
//         ASSERT_EQ(*(int*)(iter1 + 1), *(int*)(iter2 + 1));
//     }
// }

// // TEST(RbTreeTest, func3)
// // {
// //     auto tree = __RbTree_new(nullptr);
// //     std::map<int, int> stlMap;
// //     Pair p = {98, 89};
// //     int64_t st, et;
// //     int n = 10000;
// //     for (;;)
// //     {
// //         int val = rand();
// //         p.a = p.b = val;
// //         st = nanos();
// //         for (int i = 0; i < n; i++)
// //         {
// //             stlMap[i] = val;
// //         }
// //         et = nanos();
// //         cout << "stl map using " << (et - st) / 1000000 << " ms" << endl;
// //         st = nanos();
// //         for (int i = 0; i < n; i++)
// //         {
// //             __RbTree_add(tree, &p, nullptr);
// //         }
// //         et = nanos();
// //         cout << "xcl tree using " << (et - st) / 1000000 << " ms" << endl;
// //         cout << "***********" << endl << endl;
// //         __RbTree_clear(tree, nullptr);
// //         stlMap.clear();
// //         mSleep(1000);
// //     }
// // }

// #endif
