//
// Created by xuyan on 2022/7/8.
//

#include <stdlib.h>
#include <string.h>
#include "xcl/util/cleaner.h"
#include "xcl/util/CSingleList.h"

static CSingleList __cleanStack;

static void
__cleanOnExit() {
  CSingleNode* node;
  while ((node = SingList_popFront(&__cleanStack))) {
    typedef void (*ProcType)(void*);
    ProcType proc = (ProcType)((intptr_t*)node->data)[0];
    void* usr = (void*)((intptr_t*)node->data)[1];
    proc(usr);
    free(node);
  }
}

bool
__internal_registerCleanProc(void (*proc)(void*), void* usr) {
  if (SingleList_empty(&__cleanStack)) {
    if (atexit(__cleanOnExit) != 0) {
      return false;
    }
  }
  CSingleNode* node = malloc(sizeof(CSingleNode) + (sizeof(intptr_t) << 1));
  if (node) {
    memset(node, 0, sizeof(CSingleNode) + (sizeof(intptr_t) << 1));
    intptr_t* pointers = (intptr_t*)node->data;
    pointers[0] = (intptr_t)proc;
    pointers[1] = (intptr_t)usr;
    SingleList_pushFront(&__cleanStack, node);
    return true;
  }
  return false;
}
