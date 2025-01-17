#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/lang/thread.h>

typedef enum {
  WORKER_TYPE_CORE,
  WORKER_TYPE_TEMP,
} WorkerType;

typedef struct {
  struct Task* task;
  struct TaskSignal* sig;
} ExecPair;

typedef struct {
  struct Runnable super;
  struct ThreadPoolExecutor* executor;
  WorkerType type;
  ExecPair initial_pair;
  uintptr_t th;
  void* mut;
  void* cv;
} Worker;

bool Worker_start(Worker* worker);
void Worker_close(Worker* worker, bool join);
bool Worker_init(Worker* worker, ExecPair ep, struct ThreadPoolExecutor* executor);
void Worker_destroy(Worker* worker);

#ifdef __cplusplus
}
#endif
