//
// Created by 徐琰 on 2023/10/13.
//

#include <internal/lang/local_reserve_id.h>
#include <internal/lang/local_storage.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/lang/thread.h>
#include <xcl/util/log.h>

bool Local_set0(uintptr_t id, LocalPair pair);
LocalPair* Local_get0(uintptr_t id);

struct ThreadExitNode {
  struct Runnable* runnable;
  struct ThreadExitNode* next;
};

static void Thread_invokeExitChain(void* attach) {
  struct ThreadExitNode* node = attach;
  while (node) {
    run(node->runnable);
    struct ThreadExitNode* next = node->next;
    pfree(node);
    node = next;
  }
}
bool Thread_atExit(struct Runnable* runnable) {
  struct ThreadExitNode* node = palloc(sizeof(struct ThreadExitNode));
  if (node) {
    node->runnable = runnable;
    node->next = Local_get0(LOCAL_ID_EXIT_HANDLER)->attach;
    Local_set0(LOCAL_ID_EXIT_HANDLER, LocalPair_make(node, Thread_invokeExitChain));
  }
  return node;
}
