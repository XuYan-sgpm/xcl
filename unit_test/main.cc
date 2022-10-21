//
// Created by 徐琰 on 2022/7/24.
//

#include <gtest/gtest.h>
#include <xcl/util/rb_tree.h>
#include <assert.h>
#include <stdlib.h>
#include <map>
#include <xcl/lang/System.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <xcl/util/Map.h>
using namespace std;

struct MapComparator {
    bool
    operator()(const int& lhs, const int& rhs) const
    {}
};

struct A {
protected:
    ~A()
    {
        cout << "A destructor" << endl;
    }
};

void
print(bool isXcl, int64_t time)
{
    static mutex m;
    static double xcl_total = 0, stl_total = 0;
    static int xcl_count = 0, stl_count = 0;
    // static int64_t values[2] = {-1};
    m.lock();
    if (isXcl)
    {
        cout << "------xcl tree use: " << time << " ms\n";
    }
    else
    {
        cout << "stl map use: " << time << " ms\n";
    }
    m.unlock();
}

#define N 5000000
static int* values = nullptr;

static bool
initValues()
{
    if (values == nullptr)
    {
        values = new int[N];
    }
    for (int i = 0; i < N; i++)
    {
        values[i] = rand();
    }
    return true;
}

static bool flag = initValues();

void
func()
{
    A* a;
}

int
main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();

    auto func1 = [&] {
        //
    };

    auto func2 = [&] {
        std::map<int, int> stlMap;
        int64_t st, et;
        for (;;)
        {
            st = nanos();
            for (int i = 0; i < N; i++)
            {
                auto p = make_pair(values[i], values[i]);
                stlMap.insert(p);
            }
            et = nanos();
            print(false, (et - st) / 1000000);
            stlMap.clear();
            mSleep(1000);
        }
    };

    thread t1(func1);
    thread t2(func2);
    t1.join();
    t2.join();
}