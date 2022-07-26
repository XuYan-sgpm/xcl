//
// Created by xuyan on 2022/6/21.
//

#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CThread.h"
#include "xcl/lang/CThreadLocal.h"
#include <gtest/gtest.h>

#include <iostream>
#include <xcl/lang/system.h>

using namespace std;

static void
__testThreadLocal(CThreadLocal* local)
{
    const char* p = "hello world";
    assert(Local_set(local, (void*)p));
    void* ptr;
    assert(Local_get(local, &ptr));
    assert(ptr == p);
    assert(Local_setInt32(local, 89));
    int32_t val;
    assert(Local_getInt32(local, &val));
    assert(val == 89);
    float val2;
    assert(Local_setFloat(local, 3.1415926f));
    assert(Local_getFloat(local, &val2));
    assert(val2 == 3.1415926f);
}

void
threadProc(void* args)
{
    CThreadLocal locals[10];
    for (int i = 0; i < 10; i++)
    {
        locals[i] = Local_make();
    }
    for (int loop = 0; loop < 1000; loop++)
    {
        for (int i = 0; i < 10; i++)
        {
            __testThreadLocal(locals + i);
        }
    }
    *(int*)args = 0;
}

TEST(ThreadLocal, func1)
{
    int n = 4;
    CThread threads[n];
    int code[n];
    for (int i = 0; i < n; i++)
    {
        code[i] = -1;
    }
    for (int i = 0; i < n; i++)
    {
        threads[i] = Thread_create(threadProc, code + i);
        ASSERT_TRUE(Thread_valid(threads[i]));
    }
    for (int i = 0; i < n; i++)
    {
        Thread_join(&threads[i]);
    }
    for (int i = 0; i < n; i++)
    {
        ASSERT_EQ(code[0], 0);
    }
}

typedef struct {
    CThreadLocal* local;
    int32_t val;
} __TestLocalParameter;

static void
__threadProc2(void* args)
{
    cout << "in thread proc:" << Thread_currentId() << endl;
    __TestLocalParameter* parameter = static_cast<__TestLocalParameter*>(args);
    CThreadLocal* local = parameter->local;
    int32_t val;
    Local_setInt32(local, parameter->val);
    mSleep(1000);
    ASSERT_TRUE(Local_getInt32(local, &val));
    ASSERT_EQ(val, parameter->val);
}

TEST(ThreadLocal, func2)
{
    int n = 1;
    CThread threads[n];
    __TestLocalParameter parameters[n];
    CThreadLocal local = Local_make();
    for (int i = 0; i < n; i++)
    {
        parameters[i] = {&local, 45 + i};
        threads[i] = Thread_create(__threadProc2, parameters + i);
        ASSERT_TRUE(Thread_valid(threads[i]));
    }
    for (int i = 0; i < n; i++)
    {
        Thread_join(&threads[i]);
    }
}
