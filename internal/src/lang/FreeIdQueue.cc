//
// Created by xuyan on 2022/6/22.
//

#include <concurrent/Lock.h>
#include <cstring>
using namespace std;

class FreeIdQueue {
 private:
  mutable xcl::Lock *lock_;
  int64_t *buf_;
  uint32_t cap_;
  uint32_t size_;

 public:
  FreeIdQueue();
  ~FreeIdQueue();

 public:
  bool offer(int64_t id);
  bool isEmpty() const;
  bool poll(int64_t *id);
};
FreeIdQueue::FreeIdQueue()
    : lock_(xcl::Lock::NewLock()), buf_(nullptr), cap_(0), size_(0) {}
FreeIdQueue::~FreeIdQueue() {
  if (buf_) {
    delete[] buf_;
    buf_ = nullptr;
    cap_ = size_ = 0;
  }
}
bool FreeIdQueue::offer(int64_t id) {
  xcl::Locker locker(lock_);
  if (size_ == cap_) {
    if (cap_ == 0) {
      cap_ = 8;
    } else {
      cap_ <<= 1u;
    }
    int64_t *newBuf = new int64_t[cap_];
    memcpy(newBuf, buf_, size_ * sizeof(int64_t));
    delete[] buf_;
    buf_ = newBuf;
  }
  buf_[size_++] = id;
  return true;
}
bool FreeIdQueue::isEmpty() const {
  xcl::Locker locker(lock_);
  return size_ == 0;
}
bool FreeIdQueue::poll(int64_t *id) {
  xcl::Locker locker(lock_);
  if (size_ == 0) {
    return false;
  }
  *id = buf_[--size_];
  return true;
}

static FreeIdQueue __idQueue;

#ifdef __cplusplus
extern "C" {
#endif

bool offerFreeId(int64_t id) { return __idQueue.offer(id); }
bool hasFreeId() { return __idQueue.isEmpty(); }
bool pollFreeId(int64_t *id) { return __idQueue.poll(id); }

#ifdef __cplusplus
}
#endif