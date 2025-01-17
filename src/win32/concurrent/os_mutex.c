//
// Created by xuyan on 2022/7/4.
//

#include <Windows.h>
#include <internal/concurrent/atomic.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/lang/system.h>

bool Mutex_init(void* obj) {
  InitializeCriticalSection(obj);
  return true;
}
void* Mutex_new() {
  void* mutex = malloc(sizeof(CRITICAL_SECTION));
  if (mutex)
    Mutex_init(mutex);
  return mutex;
}

void Mutex_destroy(void* obj) { DeleteCriticalSection(obj); }

bool Mutex_delete(void* mutex) {
  if (mutex) {
    Mutex_destroy(mutex);
    free(mutex);
    return true;
  }
  return false;
}

bool Mutex_lock(void* mutex) { return mutex ? (EnterCriticalSection(mutex), true) : false; }

bool Mutex_unlock(void* mutex) { return mutex ? LeaveCriticalSection(mutex), true : false; }

bool Mutex_tryLock(void* mutex) { return !mutex ? false : TryEnterCriticalSection(mutex); }

bool Mutex_timedLock(void* mutex, int32_t millis) {
  bool acquired = false;
  if (mutex) {
    unsigned wait_limit = millis < 0 ? 0xffffffff : millis;
    int64_t deadline = nanos() + wait_limit * 1000000;
    int64_t now;
    while (!(acquired = TryEnterCriticalSection(mutex)) && (now = nanos()) < deadline)
      Sleep(1);
  }
  return acquired;
}
