#ifdef __linux__
#include <signal.h>
#include <sys/syscall.h>
#endif
#include <errno.h>
#include <internal/lang/local_reserve_id.h>
#include <internal/lang/local_storage.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/lang/thread.h>
#include <xcl/util/log.h>

bool Local_set0(uintptr_t id, LocalPair pair);
LocalPair* Local_get0(uintptr_t id);

#if SIZEOF_PTHREAD == 8
static pthread_t handle2pthread(uintptr_t handle) { return (pthread_t)handle; }
static uintptr_t pthread2handle(pthread_t pt) { return (uintptr_t)pt; }
static void _closeThreadHandle(uintptr_t handle) {}
uintptr_t Thread_current() { return (uintptr_t)pthread_self(); }
#else
static pthread_t handle2pthread(uintptr_t handle) { return *(pthread_t*)handle; }
static uintptr_t pthread2handle(pthread_t pt) {
  pthread_t* o = palloc(SIZEOF_PTHREAD);
  *o = pt;
  return (uintptr_t)o;
}
static void _closeThreadHandle(uintptr_t handle) { pfree((void*)handle); }
uintptr_t Thread_current() {
  uintptr_t handle = (uintptr_t)Local_get0(LOCAL_ID_CURR_THREAD)->attach;
  if (!handle || handle == INVALID_THREAD_HANDLE) {
    handle = pthread2handle(pthread_self());
    Local_set0(LOCAL_ID_CURR_THREAD, LocalPair_make((void*)handle, pfree));
  }
  return handle;
}
#endif

static void* Thread_routine(void* args) {
  run(args);
  return NULL;
}

uintptr_t Thread_create(struct Runnable* runnable) {
  pthread_t pt;
  if (pthread_create(&pt, NULL, Thread_routine, runnable))
    return INVALID_THREAD_HANDLE;
  return pthread2handle(pt);
}

bool Thread_join(uintptr_t thread) {
  if (thread == INVALID_THREAD_HANDLE)
    return false;
  int err = pthread_join(handle2pthread(thread), NULL);
  if (err)
    return errno = err, false;
  _closeThreadHandle(thread);
  return true;
}

int Thread_join2(uintptr_t thread, unsigned ms) {
  if (thread == INVALID_THREAD_HANDLE)
    return false;
  int64_t now;
  int64_t deadline = currentMillis() + ms;
  pthread_t pt = handle2pthread(thread);
  int err;
  while ((err = pthread_kill(pt, 0)) == 0 && (now = currentMillis()) < deadline)
    usleep(1000);
  if (err == ESRCH && pthread_detach(pt) == 0)
    return _closeThreadHandle(thread), 1;
  return err == 0 ? 0 : -1;
}

bool Thread_detach(uintptr_t thread) {
  if (thread == INVALID_THREAD_HANDLE)
    return false;
  int err = pthread_detach(handle2pthread(thread));
  if (err)
    return errno = err, false;
  _closeThreadHandle(thread);
  return true;
}

#if defined(Darwin) || defined(iOS)
unsigned long Thread_currentId() {
  uint64_t id;
  pthread_threadid_np(NULL, &id);
  return id;
}
#elif defined(__linux__)
unsigned long Thread_currentId() {
  pid_t id;
  id = syscall(SYS_gettid);
  return id;
}
#else
#error no current thread id support
#endif

int Thread_alive(uintptr_t thread) {
  if (thread == INVALID_THREAD_HANDLE)
    return -1;
  int err = pthread_kill(handle2pthread(thread), 0);
  if (err && err != ESRCH)
    return errno = err, -1;
  return err == 0;
}
