//
// Created by xuyan on 2022/7/6.
//

#include <stddef.h>
#include <stdint.h>
#include <process.h>
#include "lang/CThread.h"
#include "util/CSingleList.h"
#include <stdlib.h>

const static int32_t __THREAD_SUSPEND = 0x10;
const static int32_t __THREAD_RUNNING = 0X11;
const static int32_t __THREAD_DEAD = 0X12;

struct _CThread_st {
  const uintptr_t handle;
  void *const threadLock;
  int32_t status;
  CSingleList callStack;
};

XCL_PUBLIC CThread *XCL_API Thread_new(bool suspend) {
  CThread *thread = malloc(sizeof(CThread));
  if (thread) {
    
  }
  return thread;
}