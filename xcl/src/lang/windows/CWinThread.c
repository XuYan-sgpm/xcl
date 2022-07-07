//
// Created by xuyan on 2022/7/6.
//

#include <stddef.h>
#include <stdint.h>
#include <process.h>
#include "lang/CThread.h"
#include "util/CSingleList.h"
#include <stdlib.h>

typedef enum { SUSPEND, ALIVE, DEAD } CThreadState;

struct _CThread_st {
  const uintptr_t handle;
  void* const threadLock;
  CThreadState status;
  unsigned threadId;
  CSingleList callStack;
};

XCL_PUBLIC CThread* XCL_API
Thread_new(bool suspend, unsigned (*threadProc)(void*), void* usr) {
  CThread* thread = malloc(sizeof(CThread));
  if (thread) {
    unsigned tid;
    uintptr_t h = _beginthreadex(NULL, 0, threadProc, usr, suspend, &tid);
  }
  return thread;
}