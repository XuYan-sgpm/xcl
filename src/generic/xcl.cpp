#include <internal/lang/local_storage.h>
#include <internal/lang/os_local.h>
#include <internal/util/log_impl.h>
#include <internal/xcl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/concurrent/condition.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/lang/system.h>

extern "C" {
void* __xcl_mutex = nullptr;
void* __xcl_cv = nullptr;
unsigned long __xcl_key = INVALID_LOCAL_KEY;
LogImpl* __xcl_log = nullptr;
HandleTable* __xcl_handle_table = nullptr;
HandlePool* __xcl_handle_pool = nullptr;
static struct {
  void (*callbacks[MAX_EXIT_HANDLER])();
  unsigned size;
} __xcl_exit_list = {0};

void HandlePool_globalInit();

static void Xcl_finalize() {
  HandleTable_free(__xcl_handle_table);
  HandlePool_free(__xcl_handle_pool);
  if (__xcl_log) {
    LogImpl_destroy(__xcl_log);
    free(__xcl_log);
  }
  if (__xcl_key != INVALID_LOCAL_KEY)
    destroyLocalKey(__xcl_key);
  Cond_delete(__xcl_cv);
  Mutex_delete(__xcl_mutex);
}

static void Xcl_resetMainThreadLocal() {
  if (__xcl_key == INVALID_LOCAL_KEY)
    return;
  void* obj = osLocalGet(__xcl_key);
  if (obj) {
    LocalStorage_free(obj);
    osLocalSet(__xcl_key, nullptr);
  }
}

static void Xcl_newMutex() {
  while (!__xcl_mutex)
    __xcl_mutex = Mutex_new();
}

static void Xcl_newCv() {
  while (!__xcl_cv)
    __xcl_cv = Cond_new();
}

static void Xcl_newLocalKey() {
  while (__xcl_key == INVALID_LOCAL_KEY)
    __xcl_key = allocLocalKey();
}

static void Xcl_newLog() {
  while (!__xcl_log)
    __xcl_log = (LogImpl*)malloc(sizeof(LogImpl));
  LogImpl_init(__xcl_log);
}

static void Xcl_newHandlePool() {
  while (!__xcl_handle_pool)
    __xcl_handle_pool = HandlePool_new();
}

static void Xcl_newNHTable() {
  Xcl_init(XCL_HANDLE_POOL);
  while (!__xcl_handle_table)
    __xcl_handle_table = HandleTable_new(__xcl_handle_pool);
}

typedef struct {
  bool done;
  void (*const f)();
} XclInitPair;

void Xcl_init(XclItem item) {
  static XclInitPair __xcl_init_list[] = {
      {false, Xcl_newMutex}, {false, Xcl_newCv},         {false, Xcl_newLocalKey},
      {false, Xcl_newLog},   {false, Xcl_newHandlePool}, {false, Xcl_newNHTable},
  };
  XclInitPair* pair = &__xcl_init_list[item];
  if (!pair->done) {
    pair->f();
    pair->done = true;
  }
}

bool postExit(void (*f)()) {
  if (!f || __xcl_exit_list.size >= MAX_EXIT_HANDLER)
    return false;
  __xcl_exit_list.callbacks[__xcl_exit_list.size++] = f;
  return true;
}
}

struct Xcl {
  Xcl() {
    Xcl_init(XCL_MUTEX);
    Xcl_init(XCL_CV);
    Xcl_init(XCL_LOCAL_KEY);
    Xcl_init(XCL_LOG);
    Xcl_init(XCL_HANDLE_POOL);
    Xcl_init(XCL_NH_TABLE);
    HandlePool_globalInit();
    LOG_P(LOG_INFO, "xcl module done, std: %lu, version: %s", __xcl_std__, __xcl_version__);
  }
  ~Xcl() {
    Xcl_resetMainThreadLocal();
    for (unsigned i = 0; i < __xcl_exit_list.size; ++i)
      __xcl_exit_list.callbacks[i]();
    Xcl_finalize();
  }
};
static Xcl xcl;
