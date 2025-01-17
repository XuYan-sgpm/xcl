#include <gtest/gtest.h>
#include <xcl/lang/system.h>

#include <chrono>
#include <thread>

TEST(SleepTest, test1) {
  printf("using stl sleep_for\n");
  for (int i = 0; i < 100; i++) {
    // auto st = std::chrono::steady_clock::now();
    auto st = nanos();
    // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    msleep(1);
    // auto et = std::chrono::steady_clock::now();
    auto et = nanos();
    // printf("sleep cost %d ms\n",
    //        std::chrono::duration_cast<std::chrono::milliseconds>(et - st)
    //            .count());
    printf("sleep cost %d ms\n", (et - st) / 1000000);
  }
}
