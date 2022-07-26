//
// Created by xuyan on 2022/7/10.
//

#include "xcl/lang/system.h"
#include <gtest/gtest.h>
#include <iostream>
#include <xcl/lang/CThread.h>

using namespace std;

void XCL_API
__threadProc(void*)
{
    cout << "enter thread" << endl;
    mSleep(3000);
    cout << "sleep finished" << endl;
    CThread currentThread = Thread_current();
}

TEST(TestThread, func1)
{
    CThread thread;
    thread = Thread_create(__threadProc, NULL);
    cout << "before main sleep" << endl;
    mSleep(1000);
    cout << "main sleep after" << endl;
    Thread_join(&thread);
}