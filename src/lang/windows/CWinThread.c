//
// Created by xuyan on 2022/7/6.
//

#include <stddef.h>
#include <stdint.h>
#include <windows.h>
#include <process.h>
#include "lang/CThread.h"
#include "util/CSingleList.h"
#include "concurrent/CMutex.h"
#include <stdlib.h>

typedef enum { SUSPEND, ALIVE, TERMINATED, INVALID } CThreadState;

typedef struct {
  unsigned (*const cb)(void*);
  void* const usr;
} CbObj;

struct _CThread_st {
  HANDLE const handle;
  void* const threadLock;
  CThreadState state;
  const unsigned threadId;
  CSingleList callStack;
  CbObj main;
  unsigned exitCode;
};

//static unsigned __stdcall

XCL_PUBLIC CThread* XCL_API
Thread_new(bool suspend, ThreadProc threadProc, void* usr) {
  CThread* thread = malloc(sizeof(CThread));
  if (thread) {
    memset(thread, 0, sizeof(CThread));
    unsigned tid;
    thread->state = INVALID;
    HANDLE h = (HANDLE)_beginthreadex(
        NULL, 0, threadProc, usr, suspend ? CREATE_SUSPENDED : 0, &tid);
    if (h != 0) {
      void* lock = Mutex_new();
      if (lock) {
        memcpy((void*)&thread->threadLock, (void*)&lock, sizeof(void*));
        memcpy((void*)&thread->handle, (void*)&h, sizeof(HANDLE));
        memcpy((void*)&thread->threadId, (void*)&tid, sizeof(unsigned));
        CbObj threadProcCb = {.cb = threadProc, .usr = usr};
        memcpy((void*)&thread->main, (void*)&threadProcCb, sizeof(CbObj));
        thread->exitCode = 0;
        return thread;
      } else {
        _endthreadex(-1);
        CloseHandle(h);
      }
    }
    free(thread);
  }
  return NULL;
}