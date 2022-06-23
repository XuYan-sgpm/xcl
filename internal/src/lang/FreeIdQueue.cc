//
// Created by xuyan on 2022/6/22.
//

#include <mutex>
#include <cstring>
using namespace std;

class FreeIdQueue {
 private:
  mutable mutex lock_;
  long *buf_;
  uint32_t cap_;
  uint32_t size_;

 public:
  FreeIdQueue();
  ~FreeIdQueue();

 public:
  bool offer(long id);
  bool isEmpty() const;
  bool poll(long *id);
};
FreeIdQueue::FreeIdQueue() : lock_(), buf_(nullptr), cap_(0), size_(0) {}
FreeIdQueue::~FreeIdQueue() {
  if (buf_) {
    delete[] buf_;
    buf_ = nullptr;
    cap_ = size_ = 0;
  }
}
bool FreeIdQueue::offer(long id) {
  lock_guard<mutex> guard(lock_);
  if (size_ == cap_) {
    if (cap_ == 0) {
      cap_ = 8;
    } else {
      cap_ <<= 1u;
    }
    long *newBuf = new long[cap_];
    memcpy(newBuf, buf_, size_ * sizeof(long));
    delete[] buf_;
    buf_ = newBuf;
  }
  buf_[size_++] = id;
  return true;
}
bool FreeIdQueue::isEmpty() const {
  lock_guard<mutex> guard(lock_);
  return size_ == 0;
}
bool FreeIdQueue::poll(long *id) {
  lock_guard<mutex> guard(lock_);
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

bool offerFreeId(long id) { return __idQueue.offer(id); }
bool hasFreeId() { return __idQueue.isEmpty(); }
bool pollFreeId(long *id) { return __idQueue.poll(id); }

#ifdef __cplusplus
}
#endif