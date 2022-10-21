#include <iostream>
#include <gtest/gtest.h>
#include <xcl/lang/handle_table.h>
#include <stdlib.h>

TEST(HandleTest, func1)
{
    struct HandleTable* table = HandleTable_new(Pool_def());
    ASSERT_NE(table, nullptr);
    int count = 1024;
    int* array = (int*)alloca(count * sizeof(int));
    ASSERT_NE(array, nullptr);
    for (int i = 0; i < count; i++)
    {
        array[i] = rand();
    }
    void** handles = (void**)alloca(count * sizeof(void*));
    ASSERT_NE(handles, nullptr);
    memset(handles, 0, sizeof(void*) * count);
    for (int i = 0; i < count; i++)
    {
        handles[i] = HandleTable_add(table, array + i, NULL);
        if (!handles[i])
        {
            std::cout << "handle nullptr" << std::endl;
        }
        ASSERT_NE(handles[i], nullptr);
        std::cout << "handle:" << handles[i] << std::endl;
    }
    for (int i = 0; i < count; i++)
    {
        ASSERT_EQ(*(int*)HandleTable_get(table, handles[i]), array[i]);
    }
    HandleTable_delete(table);
}

TEST(HandleTest, func2)
{
    struct HandleTable* table = HandleTable_new(Pool_def());
    ASSERT_NE(table, nullptr);
    int count = 512;
    int* array = (int*)alloca(count * sizeof(int));
    ASSERT_NE(array, nullptr);
    for (int i = 0; i < count; i++)
    {
        array[i] = rand();
    }
    void** handles = (void**)alloca(count * sizeof(void*));
    ASSERT_NE(handles, nullptr);
    memset(handles, 0, sizeof(void*) * count);
    for (int i = 0; i < count; i++)
    {
        handles[i] = HandleTable_add(table, array + i, NULL);
        if (!handles[i])
        {
            std::cout << "handle nullptr" << std::endl;
        }
        ASSERT_NE(handles[i], nullptr);
        std::cout << "handle:" << handles[i] << std::endl;
    }
    for (int i = 0; i < count; i++)
    {
        ASSERT_EQ(*(int*)HandleTable_get(table, handles[i]), array[i]);
    }
    // ASSERT_EQ(HandleTable_add(table, array + 3, nullptr, nullptr), nullptr);
    HandleTable_close(table, handles[45]);
    ASSERT_EQ(HandleTable_get(table, handles[45]), nullptr);
    auto handle = HandleTable_add(table, array + 3, nullptr);
    ASSERT_NE(handle, nullptr);
    // ASSERT_EQ(handle, handles[45]);
    handle = HandleTable_ref(table, handle);
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(*(int*)HandleTable_get(table, handle), array[3]);
    HandleTable_delete(table);
}