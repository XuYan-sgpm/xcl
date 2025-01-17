//
// Created by 14049 on 2024/5/8.
//

#pragma once

#include <stdbool.h>
#include <xcl/concurrent/util/blocking_queue.h>
#include <xcl/concurrent/util/task_signal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Task Task;
struct Task {
  struct {
    bool (*execute)(Task*);
  }* __vptr;
};

typedef struct ThreadPoolQueueFactory ThreadPoolQueueFactory;
struct ThreadPoolQueueFactory {
  const struct {
    BlockingQueue* (*new_queue)(ThreadPoolQueueFactory*, int value_size);
    void (*recycle_queue)(ThreadPoolQueueFactory*, BlockingQueue*);
  }* const __vptr;
};

typedef struct ThreadPoolExecutor ThreadPoolExecutor;
ThreadPoolExecutor* ThreadPoolExecutor_new(int core_threads, int max_threads, int keep_alive,
                                           struct ThreadPoolQueueFactory* queue_factory);
void ThreadPoolExecutor_free(ThreadPoolExecutor* executor);
TaskSignal* ThreadPoolExecutor_submit(ThreadPoolExecutor* executor, Task* task);
bool ThreadPoolExecutor_execute(ThreadPoolExecutor* executor, Task* task);

#ifdef __cplusplus
}
#endif
