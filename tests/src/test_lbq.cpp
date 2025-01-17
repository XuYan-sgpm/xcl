//
// Created by 14049 on 2024/5/5.
//

#include <gtest/gtest.h>
#include <xcl/concurrent/util/linked_blocking_queue.h>

#include <future>
#include <thread>

class LBQTest : public testing::Test {
 protected:
  BlockingQueue* bq = nullptr;

  void SetUp() {
    bq = LinkedBlockingQueue_new(4, 1000);
    ASSERT_NE(bq, nullptr);
  }
  void TearDown() { LinkedBlockingQueue_free(bq); }

  void test(int offers, int polls, int thread_test_size, int timeout = -1) {
    std::atomic<int> id_gen(0), count(0);
    const int total = offers * thread_test_size;
    auto* flags = new std::atomic<bool>[total];
    for (int i = 0; i < total; i++)
      flags[i].store(false);
    auto* threads = new std::thread[offers + polls];
    for (int i = 0; i < offers; i++) {
      threads[i] = std::thread([&] {
        for (int j = 0; j < thread_test_size; j++) {
          int val = id_gen.fetch_add(1);
          while (!BlockingQueue_offer(bq, &val)) {
          }
        }
      });
    }
    for (int i = offers; i < offers + polls; i++) {
      threads[i] = std::thread([&] {
        while (count.fetch_add(1) < total) {
          int val;
          while (!BlockingQueue_takeTimeout(bq, &val, timeout)) {
          }
          if (flags[val].exchange(true)) {
            fprintf(stderr, "%d is true\n", val);
            abort();
          }
        }
      });
    }
    for (int i = 0; i < offers + polls; i++)
      threads[i].join();
    ASSERT_TRUE(BlockingQueue_empty(bq));
    delete[] threads;
    delete[] flags;
  }
};

TEST_F(LBQTest, test1) { test(8, 4, 100000); }

TEST_F(LBQTest, test2) {
  for (int i = 0; i < 10; i++) {
    int val;
    ASSERT_FALSE(BlockingQueue_takeTimeout(bq, &val, 100));
    fprintf(stderr, "%dth lbq take timeout\n", i);
  }
}

TEST_F(LBQTest, test3) { test(4, 8, 100000, 1000); }

TEST_F(LBQTest, test4) {
  BlockingQueue* q = LinkedBlockingQueue_new(4, 0);
  ASSERT_NE(q, nullptr);
  int val;
  for (int i = 0; i < 10; i++) {
    ASSERT_FALSE(BlockingQueue_putTimeout(q, &val, 100));
    fprintf(stderr, "%dth lbq put timeout\n", i);
  }
  ASSERT_TRUE(BlockingQueue_empty(q));
  LinkedBlockingQueue_free(q);
}
