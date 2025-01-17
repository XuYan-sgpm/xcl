//
// Created by 徐琰 on 2023/10/14.
//

#include <gtest/gtest.h>
#include <internal/concurrent/atomic.h>
#include <stdint.h>
#include <xcl/lang/system.h>
#include <xcl/util/log.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

class AtomicTest : public testing::Test {
 protected:
  std::atomic<int> stl_cost, xatomic_cost;
  int stl_total, xatomic_total;

  void print_result(const char* category, int xatomic, int stl) {
    if (stl < xatomic) {
      LOG_P(LOG_DEBUG, "stl %s faster than xatomic %s %lf%c", category, category,
            (double)(xatomic - stl) * 100 / xatomic, '%');
    } else {
      LOG_P(LOG_DEBUG, "xatomic %s faster than stl %s %lf%c", category, category,
            (double)(stl - xatomic) * 100 / stl, '%');
    }
    LOG_P(LOG_DEBUG, "xatomic %d,stl %d", xatomic, stl);
  }

  void test_atomic32(int n, int times) {
    _atomic_int v1 = 0;
    std::atomic<int> v2(0);
    stl_total = xatomic_total = 0;
    auto stl_func = [&] {
      int tmp[10] = {89};
      std::chrono::steady_clock::time_point st, et;
      st = std::chrono::steady_clock::now();
      for (int i = 0; i < times; i++) {
        v2.store(tmp[0], std::memory_order_release);
        tmp[1] = v2.load(std::memory_order_acquire);
        tmp[2] = tmp[1] + 1;
        v2.compare_exchange_strong(tmp[1], tmp[2], std::memory_order_acq_rel);
        tmp[3] = 25;
        tmp[4] = v2.exchange(tmp[3], std::memory_order_acq_rel);
        tmp[5] = v2.fetch_add(1, std::memory_order_acq_rel);
        tmp[6] = v2.fetch_sub(1, std::memory_order_acq_rel);
      }
      et = std::chrono::steady_clock::now();
      stl_cost += std::chrono::duration<double, std::milli>(et - st).count();
    };

    auto xatomic_func = [&] {
      int tmp[10] = {89};
      std::chrono::steady_clock::time_point st, et;
      st = std::chrono::steady_clock::now();
      for (int i = 0; i < times; i++) {
        __atomic_store32(&v1, tmp[0], RELEASE);
        tmp[1] = __atomic_load32(&v1, ACQUIRE);
        tmp[2] = tmp[1] + 1;
        __atomic_cas32(&v1, false, &tmp[1], tmp[2], ACQ_REL);
        tmp[3] = 25;
        tmp[4] = __atomic_exchange32(&v1, tmp[3], ACQ_REL);
        tmp[5] = __atomic_fetch_add32(&v1, 1, ACQ_REL);
        tmp[6] = __atomic_fetch_sub32(&v1, 1, ACQ_REL);
      }
      et = std::chrono::steady_clock::now();
      xatomic_cost += std::chrono::duration<double, std::milli>(et - st).count();
    };
    for (int i = 0; i < 10; i++) {
      stl_cost = 0;
      xatomic_cost = 0;
      v1 = 0;
      v2 = 0;
      auto* threads = new std::thread[n << 1];
      for (int i = 0; i < n; i++)
        threads[i] = std::thread(xatomic_func);
      for (int i = n; i < (n << 1); i++)
        threads[i] = std::thread(stl_func);
      for (int i = 0; i < (n << 1); i++)
        threads[i].join();
      print_result("test", xatomic_cost.load(), stl_cost.load());
      delete[] threads;
      stl_total += stl_cost.load();
      xatomic_total += xatomic_cost.load();
    }
    print_result("total", xatomic_total, stl_total);
  }

  void test_atomic8(int n, int times) {
    _atomic_char v1 = 0;
    std::atomic<char> v2(0);
    stl_total = xatomic_total = 0;
    auto stl_func = [&] {
      char tmp[10] = {89};
      std::chrono::steady_clock::time_point st, et;
      st = std::chrono::steady_clock::now();
      for (int i = 0; i < times; i++) {
        v2.store(tmp[0], std::memory_order_release);
        tmp[1] = v2.load(std::memory_order_acquire);
        tmp[2] = tmp[1] + 1;
        v2.compare_exchange_strong(tmp[1], tmp[2], std::memory_order_acq_rel);
        tmp[3] = 25;
        tmp[4] = v2.exchange(tmp[3], std::memory_order_acq_rel);
        tmp[5] = v2.fetch_add(1, std::memory_order_acq_rel);
        tmp[6] = v2.fetch_sub(1, std::memory_order_acq_rel);
      }
      et = std::chrono::steady_clock::now();
      stl_cost += std::chrono::duration<double, std::milli>(et - st).count();
    };

    auto xatomic_func = [&] {
      char tmp[10] = {89};
      std::chrono::steady_clock::time_point st, et;
      st = std::chrono::steady_clock::now();
      for (int i = 0; i < times; i++) {
        __atomic_store8(&v1, tmp[0], RELEASE);
        tmp[1] = __atomic_load8(&v1, ACQUIRE);
        tmp[2] = tmp[1] + 1;
        __atomic_cas8(&v1, false, &tmp[1], tmp[2], ACQ_REL);
        tmp[3] = 25;
        tmp[4] = __atomic_exchange8(&v1, tmp[3], ACQ_REL);
        tmp[5] = __atomic_fetch_add8(&v1, 1, ACQ_REL);
        tmp[6] = __atomic_fetch_sub8(&v1, 1, ACQ_REL);
      }
      et = std::chrono::steady_clock::now();
      xatomic_cost += std::chrono::duration<double, std::milli>(et - st).count();
    };
    for (int i = 0; i < 10; i++) {
      stl_cost = 0;
      xatomic_cost = 0;
      v1 = 0;
      v2 = 0;
      auto* threads = new std::thread[n << 1];
      for (int i = 0; i < n; i++)
        threads[i] = std::thread(xatomic_func);
      for (int i = n; i < (n << 1); i++)
        threads[i] = std::thread(stl_func);
      for (int i = 0; i < (n << 1); i++)
        threads[i].join();
      print_result("test", xatomic_cost.load(), stl_cost.load());
      delete[] threads;
      stl_total += stl_cost.load();
      xatomic_total += xatomic_cost.load();
    }
    print_result("total", xatomic_total, stl_total);
  }

  void test_atomic16(int n, int times) {
    _atomic_short v1 = 0;
    std::atomic<short> v2(0);
    stl_total = xatomic_total = 0;
    auto stl_func = [&] {
      short tmp[10] = {89};
      std::chrono::steady_clock::time_point st, et;
      st = std::chrono::steady_clock::now();
      for (int i = 0; i < times; i++) {
        v2.store(tmp[0], std::memory_order_release);
        tmp[1] = v2.load(std::memory_order_acquire);
        tmp[2] = tmp[1] + 1;
        v2.compare_exchange_strong(tmp[1], tmp[2], std::memory_order_acq_rel);
        tmp[3] = 25;
        tmp[4] = v2.exchange(tmp[3], std::memory_order_acq_rel);
        tmp[5] = v2.fetch_add(1, std::memory_order_acq_rel);
        tmp[6] = v2.fetch_sub(1, std::memory_order_acq_rel);
      }
      et = std::chrono::steady_clock::now();
      stl_cost += std::chrono::duration<double, std::milli>(et - st).count();
    };

    auto xatomic_func = [&] {
      short tmp[10] = {89};
      std::chrono::steady_clock::time_point st, et;
      st = std::chrono::steady_clock::now();
      for (int i = 0; i < times; i++) {
        __atomic_store16(&v1, tmp[0], RELEASE);
        tmp[1] = __atomic_load16(&v1, ACQUIRE);
        tmp[2] = tmp[1] + 1;
        __atomic_cas16(&v1, false, &tmp[1], tmp[2], ACQ_REL);
        tmp[3] = 25;
        tmp[4] = __atomic_exchange16(&v1, tmp[3], ACQ_REL);
        tmp[5] = __atomic_fetch_add16(&v1, 1, ACQ_REL);
        tmp[6] = __atomic_fetch_sub16(&v1, 1, ACQ_REL);
      }
      et = std::chrono::steady_clock::now();
      xatomic_cost += std::chrono::duration<double, std::milli>(et - st).count();
    };
    for (int i = 0; i < 10; i++) {
      stl_cost = 0;
      xatomic_cost = 0;
      v1 = 0;
      v2 = 0;
      auto* threads = new std::thread[n << 1];
      for (int i = 0; i < n; i++)
        threads[i] = std::thread(xatomic_func);
      for (int i = n; i < (n << 1); i++)
        threads[i] = std::thread(stl_func);
      for (int i = 0; i < (n << 1); i++)
        threads[i].join();
      print_result("test", xatomic_cost.load(), stl_cost.load());
      delete[] threads;
      stl_total += stl_cost.load();
      xatomic_total += xatomic_cost.load();
    }
    print_result("total", xatomic_total, stl_total);
  }

  void test_atomic64(int n, int times) {
    _atomic_int64 v1 = 0;
    std::atomic<int64_t> v2(0);
    stl_total = xatomic_total = 0;
    auto stl_func = [&] {
      int64_t tmp[10] = {89};
      std::chrono::steady_clock::time_point st, et;
      st = std::chrono::steady_clock::now();
      for (int i = 0; i < times; i++) {
        v2.store(tmp[0], std::memory_order_release);
        tmp[1] = v2.load(std::memory_order_acquire);
        tmp[2] = tmp[1] + 1;
        v2.compare_exchange_strong(tmp[1], tmp[2], std::memory_order_acq_rel);
        tmp[3] = 25;
        tmp[4] = v2.exchange(tmp[3], std::memory_order_acq_rel);
        tmp[5] = v2.fetch_add(1, std::memory_order_acq_rel);
        tmp[6] = v2.fetch_sub(1, std::memory_order_acq_rel);
      }
      et = std::chrono::steady_clock::now();
      stl_cost += std::chrono::duration<double, std::milli>(et - st).count();
    };

    auto xatomic_func = [&] {
      char tmp[10] = {89};
      std::chrono::steady_clock::time_point st, et;
      st = std::chrono::steady_clock::now();
      for (int i = 0; i < times; i++) {
        __atomic_store64(&v1, tmp[0], RELEASE);
        tmp[1] = __atomic_load64(&v1, ACQUIRE);
        tmp[2] = tmp[1] + 1;
        __atomic_cas64(&v1, false, &tmp[1], tmp[2], ACQ_REL);
        tmp[3] = 25;
        tmp[4] = __atomic_exchange64(&v1, tmp[3], ACQ_REL);
        tmp[5] = __atomic_fetch_add64(&v1, 1, ACQ_REL);
        tmp[6] = __atomic_fetch_sub64(&v1, 1, ACQ_REL);
      }
      et = std::chrono::steady_clock::now();
      xatomic_cost += std::chrono::duration<double, std::milli>(et - st).count();
    };
    for (int i = 0; i < 10; i++) {
      stl_cost = 0;
      xatomic_cost = 0;
      v1 = 0;
      v2 = 0;
      auto* threads = new std::thread[n << 1];
      for (int i = 0; i < n; i++)
        threads[i] = std::thread(xatomic_func);
      for (int i = n; i < (n << 1); i++)
        threads[i] = std::thread(stl_func);
      for (int i = 0; i < (n << 1); i++)
        threads[i].join();
      print_result("test", xatomic_cost.load(), stl_cost.load());
      delete[] threads;
      stl_total += stl_cost.load();
      xatomic_total += xatomic_cost.load();
    }
    print_result("total", xatomic_total, stl_total);
  }
};

TEST_F(AtomicTest, test32) { test_atomic32(4, 100000); }

TEST_F(AtomicTest, test8) { test_atomic8(4, 100000); }

TEST_F(AtomicTest, test64) { test_atomic64(4, 100000); }

TEST_F(AtomicTest, test16) { test_atomic16(4, 100000); }

TEST_F(AtomicTest, func1) {
  _atomic_int size = 0;
  uint32_t delta = 1;
  std::thread t1([&] {
    int ret;
    for (int i = 0; i < 10000; i++) {
      ret = __atomic_fetch_add32(&size, delta, SEQ_CST);
    }
  });
  std::thread t2([&] {
    int ret;
    for (int i = 0; i < 1024; i++) {
      ret = __atomic_fetch_sub32(&size, delta, SEQ_CST);
    }
  });
  t1.join();
  t2.join();
  ASSERT_EQ(__atomic_load32(&size, SEQ_CST), 10000 - 1024);
}
