//
// Created by 徐琰 on 2022/7/24.
//

#include <gtest/gtest.h>

void
test();

int
main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();

    // auto func1 = [&] {
    //     //
    // };

    // auto func2 = [&] {
    //     std::map<int, int> stlMap;
    //     int64_t st, et;
    //     for (;;)
    //     {
    //         st = nanos();
    //         for (int i = 0; i < N; i++)
    //         {
    //             auto p = make_pair(values[i], values[i]);
    //             stlMap.insert(p);
    //         }
    //         et = nanos();
    //         print(false, (et - st) / 1000000);
    //         stlMap.clear();
    //         mSleep(1000);
    //     }
    // };

    // thread t1(func1);
    // thread t2(func2);
    // t1.join();
    // t2.join();
}