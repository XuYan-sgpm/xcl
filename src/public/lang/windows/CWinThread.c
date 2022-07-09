//
// Created by xuyan on 2022/7/6.
//

#include <stddef.h>
#include <stdint.h>
#include <Windows.h>
#include <process.h>
#include "xcl/lang/CThread.h"
#include "xcl/util/CSingleList.h"
#include "xcl/concurrent/CMutex.h"
#include "xcl/lang/CThreadLocal.h"
#include <stdlib.h>

static CThreadLocal __localThread;

typedef enum { SUSPEND, ALIVE, TERMINATED, INVALID, DETACHED } CThreadState;

typedef struct {
  unsigned (*const proc)(void*);
  void* const usr;
} ThreadProcObj;

typedef struct {
  void (*const cb)(void*);
  void* const usr;
} CallObj;

struct _CThread_st {
  const HANDLE handle;
  void* const threadLock;
  CThreadState state;
  const unsigned threadId;
  ThreadProcObj main;
  unsigned exitCode;
  CSingleList callStack;
};

static inline void
__WinThread_setThreadHandle(CThread* thread, HANDLE handle) {
  memcpy((void*)&thread->handle, (void*)&handle, sizeof(HANDLE));
}

static inline bool
__WinThread_initThreadLock(CThread* thread) {
  void* lock = Mutex_new();
  if (lock) {
    memcpy((void*)&thread->threadLock, (void*)&lock, sizeof(void*));
  }
  return lock;
}

static inline void
__WinThread_setThreadId(CThread* thread, unsigned tid) {
  memcpy((void*)&thread->threadId, (void*)&tid, sizeof(unsigned));
}

static inline bool
__WinThread_initThreadCallStack(CThread* thread) {
  CSingleNode* node = malloc(sizeof(CSingleNode) + sizeof(CallObj));
  if (node) {
    CallObj* callObj = (CallObj*)node->data;
    memset(callObj, 0, sizeof(CallObj));
    SingleList_pushBack(&thread->callStack, node);
  }
  return node;
}

static inline void
__WinThread_setThreadProc(CThread* thread, ThreadProc proc, void* usr) {
  ThreadProcObj threadProcObj = {.proc = proc, .usr = usr};
  memcpy((void*)&thread->main, (void*)&threadProcObj, sizeof(ThreadProcObj));
}

static unsigned __stdcall __WinThread_threadRun(void* args) {
  CThread* thread = args;
  if (thread->state == INVALID) {
    thread->exitCode = -1;
  } else {
    CSingleNode* node = NULL;
    while ((node = SingList_popFront(&thread->callStack))) {
      CallObj* callObj = (CallObj*)node->data;
      if (!callObj->cb) {
        thread->exitCode = thread->main.proc(thread->main.usr);
      } else {
        callObj->cb(callObj->usr);
      }
      free(node);
    }
  }
  _endthreadex(thread->exitCode);
  return thread->exitCode;
}

XCL_PUBLIC CThread* XCL_API
Thread_new(bool suspend, ThreadProc threadProc, void* usr) {
  Local_set(&__localThread, NULL);
  CThread* thread = malloc(sizeof(CThread));
  if (thread) {
    memset(thread, 0, sizeof(CThread));
    unsigned tid;
    thread->state = INVALID;
    HANDLE h = (HANDLE)_beginthreadex(
        NULL, 0, __WinThread_threadRun, thread, CREATE_SUSPENDED, &tid);
    if (h != 0) {
      if (__WinThread_initThreadLock(thread)) {
        __WinThread_setThreadHandle(thread, h);
        __WinThread_setThreadId(thread, tid);
        __WinThread_setThreadProc(thread, threadProc, usr);
        thread->exitCode = 0;
        if (__WinThread_initThreadCallStack(thread)) {
          thread->state = suspend ? SUSPEND : ALIVE;
          if (suspend)
            ResumeThread(thread->handle);
          Local_set(&__localThread, thread);
          return thread;
        }
        Mutex_delete(thread->threadLock);
      }
      ResumeThread(thread->handle);
      WaitForSingleObject(thread->handle, INFINITE);
      CloseHandle(h);
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
    DWORD tid = GetCurrentThreadId();
    thread = malloc(sizeof(CThread));
    if (thread) {
      memset(thread, 0, sizeof(CThread));
      thread->state = ALIVE;
      HANDLE h = OpenThread(SYNCHRONIZE, false, tid);
      if (h != NULL) {
        if (__WinThread_initThreadLock(thread)) {
          __WinThread_setThreadId(thread, tid);
          __WinThread_setThreadHandle(thread, h);
          Local_set(&__localThread, thread);
          return thread;
        }
        CloseHandle(h);
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
      SingleList_pushFront(&thread->callStack, node);
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
      SingleList_pushBack(&thread->callStack, node);
      ret = true;
    }
  }
  Mutex_unlock(thread->threadLock);
  return ret;
}
XCL_PUBLIC void XCL_API
Thread_delete(CThread* thread) {
  Mutex_lock(thread->threadLock);
  if (thread->state == ALIVE || thread->state == SUSPEND) {
    if (thread->state == SUSPEND) {
      ResumeThread(thread->handle);
    }
    WaitForSingleObject(thread->handle, INFINITE);
    CloseHandle(thread->handle);
  }
  Mutex_unlock(thread->threadLock);
  Mutex_delete(thread->threadLock);
  free(thread);
}
XCL_PUBLIC void XCL_API
Thread_start(CThread* thread) {
  Mutex_lock(thread->threadLock);
  if (thread->state == SUSPEND) {
    ResumeThread(thread->handle);
    thread->state = ALIVE;
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
  Mutex_lock(thread->threadLock);
  if (thread->state == ALIVE) {
    WaitForSingleObject(thread->handle, INFINITE);
    CloseHandle(thread->handle);
    thread->state = TERMINATED;
  }
  Mutex_unlock(thread->threadLock);
}
XCL_PUBLIC int32_t XCL_API
Thread_join2(CThread* thread, int32_t timeout, bool* terminated) {
  int32_t ret = -1;
  Mutex_lock(thread->threadLock);
  if (thread->state == ALIVE) {
    DWORD wait = WaitForSingleObject(thread->handle, timeout);
    if (wait == WAIT_OBJECT_0) {
      *terminated = true;
      CloseHandle(thread->handle);
      thread->state = TERMINATED;
      ret = 0;
    } else if (wait == WAIT_TIMEOUT) {
      *terminated = false;
      ret = 0;
    }
  }
  return ret;
}
XCL_PUBLIC void XCL_API
Thread_detach(CThread* thread) {
  Mutex_lock(thread->threadLock);
  if (thread->state == ALIVE) {
    CloseHandle(thread->handle);
    thread->state = DETACHED;
  }
  Mutex_unlock(thread->threadLock);
}
