//
// Created by xuyan on 2022/6/24.
//

#include "concurrent/Mutex.h"
#include "concurrent/Lock.h"
#include "lang/Thread.h"
#include "lang/XclErr.h"
#include "lang/System.h"
#include <gtest/gtest.h>
#include <iostream>

typedef void (*Callback)(void*);

using namespace std;

static void
__testLock(void* args)
{
    auto* lock = (xcl::TimedLock*)args;
    for (;;)
    {
        auto st = chrono::steady_clock::now();
        if (!lock->tryLock(500))
        {
            auto et = chrono::steady_clock::now();
            auto duration = chrono::duration<double, milli>(et - st).count();
            cout << "acquire lock failed:" << duration << endl;
        }
        else
        {
            break;
        }
    }
    //  lock->lock();
    cout << "lock successfully" << endl;
    mSleep(800);
    lock->unlock();
    cout << "unlock successfully" << endl;
}

static void
__testCMutex(void* args)
{
    for (;;)
    {
        auto st = chrono::steady_clock::now();
        //    auto st = nanos();
        if (!Mutex_tryLock2((CMutex*)args, 20))
        {
            auto et = chrono::steady_clock::now();
            //      auto et = nanos();
            auto duration = chrono::duration<double, milli>(et - st).count();
            //      auto duration = (et - st) / 1000000;
            cout << "acquire lock failed:" << duration << endl;
        }
        else
        {
            break;
        }
    }
    //  lock->lock();
    cout << "lock successfully" << endl;
    mSleep(40);
    Mutex_unlock((CMutex*)args);
    cout << "unlock successfully" << endl;
}

static void
__runLockThreads(Callback cb, void* args)
{
    int32_t nThreads = 8;
    CThread threads[8];
    for (int i = 0; i < nThreads; i++)
    {
        threads[i] = Thread_create(cb, args);
        ASSERT_TRUE(Thread_valid(threads[i]));
    }
    for (int i = 0; i < nThreads; i++)
    {
        Thread_join(&threads[i]);
    }
}

// TEST(TestLock, func1)
//{
//     xcl::TimedLock* lock = xcl::TimedLock::NewLock();
//     __runLockThreads(__testLock, lock);
// }

TEST(TestLock, func2)
{
    CMutex* mutex = Mutex_new();
    assert(mutex != NULL);
    __runLockThreads(__testCMutex, mutex);
    Mutex_delete(mutex);
}