#pragma once

#include <internal/concurrent/util/worker_set.h>
#include <stdbool.h>
#include <xcl/concurrent/util/blocking_queue.h>
#include <xcl/concurrent/util/thread_pool_executor.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

inline always_inline BlockingQueue* ThreadPoolQueueFactory_newQueue(
    ThreadPoolQueueFactory* queue_factory, int value_size) {
  return queue_factory->__vptr->new_queue(queue_factory, value_size);
}
inline always_inline void ThreadPoolQueueFactory_recycleQueue(ThreadPoolQueueFactory* queue_factory,
                                                              BlockingQueue* bq) {
  queue_factory->__vptr->recycle_queue(queue_factory, bq);
}

struct ThreadPoolExecutor {
  BlockingQueue* task_queue;
  ThreadPoolQueueFactory* queue_factory;
  unsigned tmp_worker_period;
  WorkerSet core_ws;
  WorkerSet tmp_ws;
};

#ifdef __cplusplus
}
#endif