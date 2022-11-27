#include <gtest/gtest.h>
#include <iostream>
#include <xcl/util/trie_tree.h>
#include <string>

struct A {
    struct {
        void* p;
    } * b;
};

TEST(TrieTreeTest, func1)
{
    using namespace std;
    TrieTree* tree = TrieTree_new(4, nullptr);
    ASSERT_NE(tree, nullptr);
    ASSERT_EQ(TrieTree_size(tree), 0);
    string strs[] = {"hello world",
                     "kuaiwei",
                     "xinjiang",
                     "softciphermodule",
                     "hello",
                     "xinjiang123",
                     "hello moto"};
    int n = 7;
    for (int i = 0; i < n; i++)
    {
        void* p = TrieTree_check(tree, strs[i].c_str());
        ASSERT_NE(p, nullptr);
        *(int*)p = i;
    }
    ASSERT_EQ(TrieTree_size(tree), n);
    for (int i = 0; i < n; i++)
    {
        void* p = TrieTree_get(tree, strs[i].c_str());
        ASSERT_NE(p, nullptr);
        ASSERT_EQ(*(int*)p, i);
    }
    for (int i = 0; i < n; i++)
    {
        ASSERT_TRUE(TrieTree_remove(tree, strs[i].c_str()));
        ASSERT_EQ(TrieTree_size(tree), n - i - 1);
        ASSERT_FALSE(TrieTree_contains(tree, strs[i].c_str()));
    }
    ASSERT_EQ(TrieTree_size(tree), 0);
}
