//
// Created by 徐琰 on 2024/6/28.
//

#include <internal/concurrent/atomic.h>
#include <internal/concurrent/util/executor_def.h>
#include <string.h>
#include <xcl/concurrent/util/linked_blocking_queue.h>
#include <xcl/concurrent/util/task_signal.h>
#include <xcl/concurrent/util/thread_pool_executor.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/lang/thread.h>
#include <xcl/util/log.h>

TaskSignal* TaskSignal_new();
TaskSignal* TaskSignal_ref(TaskSignal* sig);
void TaskSignal_free0(TaskSignal* sig);

static BlockingQueue* ThreadPoolExecutor_newTaskQueue(ThreadPoolExecutor* executor) {
  return executor->queue_factory
             ? ThreadPoolQueueFactory_newQueue(executor->queue_factory, sizeof(ExecPair))
             : LinkedBlockingQueue_new(sizeof(ExecPair), 255);
}
static void ThreadPoolExecutor_recycleTaskQueue(ThreadPoolExecutor* executor) {
  executor->queue_factory
      ? ThreadPoolQueueFactory_recycleQueue(executor->queue_factory, executor->task_queue)
      : LinkedBlockingQueue_free(executor->task_queue);
}

static bool ThreadPoolExecutor_tryAddWorker(ThreadPoolExecutor* executor, WorkerType worker_type,
                                            Worker* worker) {
  WorkerSet* ws = worker_type == WORKER_TYPE_CORE ? &executor->core_ws : &executor->tmp_ws;
  if (WorkerSet_add(ws, worker)) {
    worker->type = worker_type;
    if (Worker_start(worker))
      return true;
    WorkerSet_remove(ws, worker);
  }
  return false;
}

static bool ThreadPoolExecutor_trySubmit(ThreadPoolExecutor* executor, ExecPair ep) {
  int submit_state = 0;
  Worker* worker = palloc(sizeof(Worker));
  if (worker && Worker_init(worker, ep, executor)) {
    if (ThreadPoolExecutor_tryAddWorker(executor, WORKER_TYPE_CORE, worker))
      submit_state = 1;
    else if (BlockingQueue_offer(executor->task_queue, &ep))
      submit_state = 2;
    else if (ThreadPoolExecutor_tryAddWorker(executor, WORKER_TYPE_TEMP, worker))
      submit_state = 3;
    if (submit_state & 1)
      return true;
    Worker_destroy(worker);
  }
  pfree(worker);
  return submit_state > 0;
}

static bool ThreadPoolExecutor_init(ThreadPoolExecutor* executor, int core_threads, int max_threads,
                                    int keep_alive, ThreadPoolQueueFactory* queue_factory) {
  memset(executor, 0, sizeof(*executor));
  executor->queue_factory = queue_factory;
  executor->tmp_worker_period = keep_alive;
  if (WorkerSet_init(&executor->core_ws, core_threads) &&
      WorkerSet_init(&executor->tmp_ws, max_threads - core_threads) &&
      (executor->task_queue = ThreadPoolExecutor_newTaskQueue(executor)))
    return true;
  WorkerSet_destroy(&executor->core_ws);
  WorkerSet_destroy(&executor->tmp_ws);
  return false;
}
ThreadPoolExecutor* ThreadPoolExecutor_new(int core_threads, int max_threads, int keep_alive,
                                           ThreadPoolQueueFactory* queue_factory) {
  ThreadPoolExecutor* executor = palloc(sizeof(ThreadPoolExecutor));
  if (executor &&
      ThreadPoolExecutor_init(executor, core_threads, max_threads, keep_alive, queue_factory))
    return executor;
  pfree(executor);
  return NULL;
}

void ThreadPoolExecutor_free(ThreadPoolExecutor* executor) {
  if (executor) {
    ExecPair ep = {0};
    BlockingQueue_put(executor->task_queue, &ep);
    WorkerSet_destroy(&executor->core_ws);
    WorkerSet_destroy(&executor->tmp_ws);
    ThreadPoolExecutor_recycleTaskQueue(executor);
    pfree(executor);
  }
}

TaskSignal* ThreadPoolExecutor_submit(ThreadPoolExecutor* executor, Task* task) {
  TaskSignal* sig = TaskSignal_new();
  if (sig) {
    ExecPair ep = {.sig = TaskSignal_ref(sig), .task = task};
    if (!ThreadPoolExecutor_trySubmit(executor, ep)) {
      TaskSignal_free0(sig);
      sig = NULL;
    }
  }
  return sig;
}

bool ThreadPoolExecutor_execute(ThreadPoolExecutor* executor, Task* task) {
  ExecPair ep = {.sig = NULL, .task = task};
  return ThreadPoolExecutor_trySubmit(executor, ep);
}
