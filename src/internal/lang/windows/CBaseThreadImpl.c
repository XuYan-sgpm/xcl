//
// Created by xuyan on 2022/7/10.
//

#include "xcl/lang/CBaseThreadImpl.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "xcl/util/CSingleList.h"
#include "xcl/concurrent/CMutex.h"
#include "xcl/lang/CThreadLocal.h"
#include "xcl/lang/CLocalStorage.h"
#include <stdlib.h>

/**
 * position 0 used to store current CThread object
 */
static CThreadLocal __localThread = {0};

typedef struct {
  unsigned (*const proc)(void*);
  void* const usr;
} ThreadProcObj;

typedef struct {
  void (*const cb)(void*);
  void* const usr;
} CallObj;

struct _CThread_st {
  const ThreadHandle handle;
  void* const threadLock;
  CThreadState state;
  const unsigned threadId;
  ThreadProcObj main;
  unsigned exitCode;
  CSingleList* callStack;
};

static inline void
__Thread_setThreadHandle(CThread* thread, ThreadHandle handle) {
  memcpy((void*)&thread->handle, (void*)&handle, sizeof(ThreadHandle));
}

static inline bool
__Thread_initThreadLock(CThread* thread) {
  void* lock = Mutex_new();
  if (lock) {
    memcpy((void*)&thread->threadLock, (void*)&lock, sizeof(void*));
  }
  return lock;
}

static inline void
__Thread_setThreadId(CThread* thread, unsigned tid) {
  memcpy((void*)&thread->threadId, (void*)&tid, sizeof(unsigned));
}

static inline bool
__Thread_initThreadCallStack(CThread* thread) {
  thread->callStack = SingleList_new();
  CSingleNode* node = malloc(sizeof(CSingleNode) + sizeof(CallObj));
  if (node) {
    CallObj* callObj = (CallObj*)node->data;
    memset(callObj, 0, sizeof(CallObj));
    SingleList_pushBack(thread->callStack, node);
  }
  return node;
}

static inline void
__Thread_setThreadProc(CThread* thread, ThreadProc proc, void* usr) {
  ThreadProcObj threadProcObj = {.proc = proc, .usr = usr};
  memcpy((void*)&thread->main, (void*)&threadProcObj, sizeof(ThreadProcObj));
}

static inline CThreadState
__Thread_syncGetThreadState(CThread* thread) {
  Mutex_lock(thread->threadLock);
  CThreadState state = thread->state;
  Mutex_unlock(thread->threadLock);
  return state;
}

static inline void
__Thread_syncSetThreadState(CThread* thread, CThreadState state) {
  Mutex_lock(thread->threadLock);
  thread->state = state;
  Mutex_unlock(thread->threadLock);
}

void
__Thread_releaseLocalStorage() {
  CLocalStorage* localStorage = __Thread_getLocalStorage();
  if (localStorage) {
    __Thread_setLocalStorage(NULL);
    LocalStorage_free(localStorage);
  }
}

void
__Thread_setState(CThread* thread, CThreadState state) {
  thread->state = state;
}
CThreadState
__Thread_state(CThread* thread) {
  return thread->state;
}
void*
__Thread_mutex(CThread* thread) {
  return thread->threadLock;
}
ThreadHandle
__Thread_handle(CThread* thread) {
  return thread->handle;
}

static unsigned __stdcall __Thread_run(void* args) {
  CThread* thread = args;
  Local_set(&__localThread, thread);
  CSingleNode* node;
  while ((node = SingleList_popFront(thread->callStack))) {
    CallObj* callObj = (CallObj*)node->data;
    if (!callObj->cb) {
      thread->exitCode = thread->main.proc(thread->main.usr);
    } else {
      callObj->cb(callObj->usr);
    }
    free(node);
  }
  SingleList_delete(thread->callStack);
  thread->callStack = NULL;
  __Thread_releaseLocalStorage();
  __Thread_syncSetThreadState(thread, TERMINATED);
  __Thread_onFinish(thread, thread->exitCode);
  return thread->exitCode;
}

XCL_PUBLIC CThread* XCL_API
Thread_new(bool suspend, ThreadProc threadProc, void* usr) {
  bool success = false;
  CThread* thread = malloc(sizeof(CThread));
  if (thread) {
    memset(thread, 0, sizeof(CThread));
    thread->state = INVALID;
    thread->exitCode = 0;
    if (__Thread_initThreadLock(thread)) {
      if (__Thread_initThreadCallStack(thread)) {
        __Thread_beforeCreate(thread);
        __Thread_setThreadProc(thread, threadProc, usr);
        unsigned tid;
        ThreadHandle handle;
        /**
         * we set thread state to suspend before create handle
         * because if thread create successfully, all cb
         * in thread call stack will see thread's state is
         * SUSPEND. otherwise, thread create failed, and thread
         * run proc would not be executed, so we set to INVALID
         */
        thread->state = SUSPEND;
        if (__Thread_create(true, __Thread_run, thread, &handle, &tid)) {
          __Thread_setThreadId(thread, tid);
          __Thread_setThreadHandle(thread, handle);
          success = true;
        } else {
          thread->state = INVALID;
        }
        __Thread_afterCreate(thread);
        if (success) {
          if (!suspend) {
            __Thread_resume(thread);
          }
          return thread;
        }
        free(SingleList_popFront(thread->callStack));
        SingleList_delete(thread->callStack);
      }
      Mutex_delete(thread->threadLock);
    }
    free(thread);
  }
  return NULL;
}
XCL_PUBLIC CThread* XCL_API
Thread_current() {
  CThread* thread;
  if (!Local_get(&__localThread, (void**)&thread))
    return NULL;
  if (!thread) {
    /**
     * current thread must not created by Thread_new
     * so we need to wrap thread
     */
    unsigned tid = __Thread_currentId();
    thread = malloc(sizeof(CThread));
    if (thread) {
      memset(thread, 0, sizeof(CThread));
      thread->state = ALIVE;
      ThreadHandle h = __Thread_currentHandle(tid);
      if (h != NULL) {
        if (__Thread_initThreadLock(thread)) {
          __Thread_setThreadId(thread, tid);
          __Thread_setThreadHandle(thread, h);
          Local_set(&__localThread, thread);
          return thread;
        }
      }
      free(thread);
      thread = NULL;
    }
  }
  return thread;
}
XCL_PUBLIC bool XCL_API
Thread_addCbFront(CThread* thread, void (*cb)(void*), void* usr) {
  Mutex_lock(thread->threadLock);
  bool ret = false;
  if (thread->state != ALIVE) {
    CallObj callObj = {.cb = cb, .usr = usr};
    CSingleNode* node = malloc(sizeof(CSingleNode) + sizeof(CallObj));
    if (node) {
      memcpy(node->data, &callObj, sizeof(CallObj));
      SingleList_pushFront(thread->callStack, node);
      ret = true;
    }
  }
  Mutex_unlock(thread->threadLock);
  return ret;
}
XCL_PUBLIC bool XCL_API
Thread_addCbBack(CThread* thread, void (*cb)(void*), void* usr) {
  Mutex_lock(thread->threadLock);
  bool ret = false;
  if (thread->state != ALIVE) {
    CallObj callObj = {.cb = cb, .usr = usr};
    CSingleNode* node = malloc(sizeof(CSingleNode) + sizeof(CallObj));
    if (node) {
      memcpy(node->data, &callObj, sizeof(CallObj));
      SingleList_pushBack(thread->callStack, node);
      ret = true;
    }
  }
  Mutex_unlock(thread->threadLock);
  return ret;
}
XCL_PUBLIC bool XCL_API
Thread_delete(CThread* thread) {
  unsigned tid = __Thread_currentId();
  bool success = false;
  if (tid != thread->threadId) {
    CThreadState state = __Thread_syncGetThreadState(thread);
    if (state == ALIVE || state == SUSPEND) {
      if (state == SUSPEND) {
        __Thread_resume(thread);
      }
      __Thread_wait(thread);
      __Thread_finalize(thread);
    }
    __Thread_releaseLocalStorage();
    Mutex_delete(thread->threadLock);
    free(thread);
    success = true;
  }
  return success;
}
XCL_PUBLIC void XCL_API
Thread_start(CThread* thread) {
  Mutex_lock(thread->threadLock);
  if (thread->state == SUSPEND) {
    __Thread_resume(thread);
  }
  Mutex_unlock(thread->threadLock);
}
XCL_PUBLIC bool XCL_API
Thread_isAlive(CThread* thread) {
  Mutex_lock(thread->threadLock);
  bool ret = thread->state == ALIVE;
  Mutex_unlock(thread->threadLock);
  return ret;
}
XCL_PUBLIC void XCL_API
Thread_join(CThread* thread) {
  if (__Thread_syncGetThreadState(thread) == ALIVE) {
    __Thread_wait(thread);
    __Thread_finalize(thread);
  }
}
XCL_PUBLIC int32_t XCL_API
Thread_join2(CThread* thread, int32_t timeout, bool* terminated) {
  int32_t ret = -1;
  if (__Thread_syncGetThreadState(thread) == ALIVE) {
    if (__Thread_waitTimeout(thread, timeout)) {
      *terminated = true;
      __Thread_finalize(thread);
      ret = 0;
    }
  }
  return ret;
}
XCL_PUBLIC void XCL_API
Thread_detach(CThread* thread) {
  Mutex_lock(thread->threadLock);
  if (thread->state == ALIVE) {
    __Thread_detach(thread);
  }
  Mutex_unlock(thread->threadLock);
}
