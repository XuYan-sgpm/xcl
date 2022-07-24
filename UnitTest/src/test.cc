//
// Created by 徐琰 on 2022/7/24.
//

#include <gtest/gtest.h>
#include "xcl/lang/CThread.h"
#include "xcl/lang/system.h"
#include <iostream>
using namespace std;

void __threadProc(void*)
{
    sleepMillis(2000);
    cout << "in thread proc" << endl;
}

TEST(Test1, func1)
{
    CThread thread = Thread_create(__threadProc, nullptr);
    Thread_join(&thread);
}