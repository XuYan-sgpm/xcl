//
// Created by 徐琰 on 2022/2/9.
//
#include <unistd.h>
#include <xcl/lang/thread.h>
#include <xcl/lang/thread_local.h>
#include <iostream>
extern "C" {
class Wrapper : public xcl::Runnable {
  void (*func_)(void) = nullptr;

 public:
  Wrapper(void (*func)()) : func_(func) {}

 public:
  void
  Run() override {
    func_();
  }
};
#include <xcl/lang/copy_handle.h>
#include <xcl/util/time_unit.h>
void *
TestCurrentThread(void *arg) {
  auto h = pthread_self();
  std::cout << "handle: " << h << std::endl;
  //    pthread_detach(pair_);
  auto thread = xcl::Thread::CurrentThread();
  class B : public xcl::Runnable {
   public:
    void
    Run() override {
      std::cout << "B run" << std::endl;
    }
  };
  std::cout << std::boolalpha << thread.AddCallback(new B()) << std::endl;
  //  thread->DetachThread();
  //  std::cout << "finished" << std::endl;
  thread.Join();
  //    pthread_join(pair_, nullptr);
  //  return thread;
  return nullptr;
}
void
TestThread() {
  auto thread1 = xcl::Thread::NewThread(new Wrapper([]() {
    sleep(3);
    std::cout << "thread finished" << std::endl;
  }));
  thread1.AddCallback(new Wrapper([]() {
    sleep(1);
    std::cout << "clean" << std::endl;
  }));
  thread1.Detach();
  sleep(5);
}
void
TestThreadLocal() {
  class Integer : public xcl::Object {
   private:
    int value_ = 0;

   public:
    Integer(int value) : value_(value) {}

   public:
    operator int &() noexcept { return value_; }
    int
    value() const {
      return value_;
    }
  };
  Integer integer(12);
  std::cout << &integer << std::endl;
  xcl::ThreadLocal local;
  local.Set(&integer);
  auto object = local.Get();
  std::cout << object << std::endl;
  auto ptr = static_cast<Integer *>(object);
  if (!ptr) {
    std::cerr << "not object\n";
    return;
  }
  std::cout << ptr->value() << '\n';
  std::cout << *ptr << '\n';
  xcl::ThreadLocal local2;
  local2.Set(nullptr);
  std::cout << local2.Get() << std::endl;
}
}
int
main() {
  TestThread();
  return 0;
}
