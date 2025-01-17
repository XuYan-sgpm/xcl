#pragma once

#include <internal/concurrent/util/worker.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int limit;
  void* mut;
  void* set;
} WorkerSet;

bool WorkerSet_init(WorkerSet* ws, int limit);
void WorkerSet_destroy(WorkerSet* ws);
bool WorkerSet_add(WorkerSet* ws, Worker* worker);
bool WorkerSet_remove(WorkerSet* ws, Worker* worker);

#ifdef __cplusplus
}
#endif
