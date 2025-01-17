//
// Created by 徐琰 on 2023/10/20.
//

#include <gtest/gtest.h>
#include <internal/concurrent/atomic.h>
#include <xcl/concurrent/util/concurrent_linked_queue.h>
#include <xcl/lang/system.h>

#include <atomic>
#include <chrono>
#include <thread>

class CLQTest : public ::testing::Test {
  void _offer() {
    for (int i = 0; i < per_count_; i++) {
      int val = id_gen_.fetch_add(1);
      ASSERT_FALSE(flags_[val].load());
      ASSERT_TRUE(CLQ_offer(queue_, val));
    }
  }

  void _poll() {
    for (;;) {
      if (size_.load() == offers_ * per_count_)
        break;
      uint64_t val;
      if (!CLQ_poll(queue_, &val))
        continue;
      ASSERT_LE(val, offers_ * per_count_);
      if (flags_[val].load()) {
        printf("position %d is true??\n", val);
        ASSERT_FALSE(true);
      }
      flags_[val].store(true);
      size_.fetch_add(1);
    }
  }

 protected:
  static void* queue_;
  unsigned per_count_, offers_, polls_;
  std::atomic<int> id_gen_;
  std::atomic<int> size_;
  std::thread* threads_;
  std::atomic<bool>* flags_;

  std::atomic<uint64_t>* durations_;
  std::mutex lock_;
  std::condition_variable cond_;

  static void SetUpTestCase() { queue_ = CLQ_new(); }
  static void TearDownTestCase() { CLQ_delete(queue_); }

  void SetUp() {
    id_gen_ = 0;
    size_ = 0;
  }

  void TearDown() {
    delete[] threads_;
    delete[] flags_;
  }

  void test_queue(int offers, int polls, int per_count) {
    offers_ = offers;
    polls_ = polls;
    per_count_ = per_count;
    threads_ = new std::thread[offers_ + polls_];
    flags_ = new std::atomic<bool>[offers_ * per_count_];
    for (int i = 0; i < offers_ * per_count_; i++)
      flags_[i].store(false);
    for (int i = offers_; i < offers_ + polls_; i++)
      threads_[i] = std::thread(&CLQTest::_poll, this);
    for (int i = 0; i < offers_; i++)
      threads_[i] = std::thread(&CLQTest::_offer, this);
    for (int i = offers_; i < offers_ + polls_; i++)
      threads_[i].join();
    for (int i = 0; i < offers_; i++)
      threads_[i].join();

    ASSERT_EQ(size_.load(), offers_ * per_count_);
  }
};

void* CLQTest::queue_ = nullptr;

TEST_F(CLQTest, func1) { test_queue(8, 4, 10000); }
