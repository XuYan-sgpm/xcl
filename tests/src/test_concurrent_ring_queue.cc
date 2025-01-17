//
// Created by 徐琰 on 2023/10/22.
//

#include <gtest/gtest.h>
#include <xcl/concurrent/util/concurrent_ring_queue.h>

#include <atomic>
#include <thread>

class CRQTest : public ::testing::Test {
  void _offer() {
    for (int i = 0; i < per_count_; i++) {
      int val = id_gen_.fetch_add(1);
      ASSERT_FALSE(flags_[val].load());
      while (!CRQ_offer(queue_, val)) {
      }
    }
  }

  void _poll() {
    for (;;) {
      if (size_.load() == offers_ * per_count_)
        break;
      uint64_t val;
      if (!CRQ_poll(queue_, &val))
        continue;
      ASSERT_LE(val, offers_ * per_count_);
      ASSERT_FALSE(flags_[val].load());
      flags_[val].store(true);
      size_.fetch_add(1);
    }
  }

 protected:
  void* queue_;
  std::atomic<int> id_gen_;
  std::atomic<int> size_;
  std::thread* threads_;
  std::atomic<bool>* flags_;
  unsigned offers_, polls_;
  unsigned per_count_;

  void SetUp() override {
    Test::SetUp();
    queue_ = CRQ_new(1 << 15);
    id_gen_ = 0;
    size_ = 0;
  }

  void TearDown() override {
    Test::TearDown();
    CRQ_delete(queue_);
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
      threads_[i] = std::thread(&CRQTest::_poll, this);
    for (int i = 0; i < offers_; i++)
      threads_[i] = std::thread(&CRQTest::_offer, this);
    for (int i = 0; i < offers_; i++)
      threads_[i].join();
    // printf("offer finished\n");
    for (int i = offers_; i < offers_ + polls_; i++)
      threads_[i].join();
    // printf("poll finished\n");
    ASSERT_EQ(size_.load(), offers_ * per_count_);
  }
};

TEST_F(CRQTest, func) { test_queue(8, 4, 10000); }
