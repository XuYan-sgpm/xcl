#include <internal/concurrent/util/executor_def.h>
#include <string.h>
#include <xcl/concurrent/condition.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/concurrent/util/blocking_queue.h>
#include <xcl/concurrent/util/task_signal.h>
#include <xcl/concurrent/util/thread_pool_executor.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/log.h>

static void execAndSignal(ExecPair ep) {
  bool ret = ep.task->__vptr->execute(ep.task);
  if (ep.sig) {
    TaskSignal_trigger(ep.sig, ret ? TASK_STATE_COMPLETE : TASK_STATE_ERROR);
    TaskSignal_free(ep.sig);
  }
}

static const char* Worker_typeName(WorkerType worker_type) {
  static const char* const worker_type_names[] = {"core", "tmp"};
  return worker_type_names[worker_type];
}

static bool Worker_getTask(Worker* worker, ExecPair* ep) {
  return worker->type == WORKER_TYPE_CORE
             ? (BlockingQueue_take(worker->executor->task_queue, ep), true)
             : BlockingQueue_takeTimeout(worker->executor->task_queue, ep,
                                         worker->executor->tmp_worker_period);
}

bool Worker_start(Worker* worker) {
  Mutex_lock(worker->mut);
  worker->th = Thread_create(&worker->super);
  bool done = worker->th != INVALID_THREAD_HANDLE;
  Mutex_unlock(worker->mut);
  if (done)
    Cond_signal(worker->cv);
  return done;
}

void Worker_close(Worker* worker, bool join) {
  join ? Thread_join(worker->th) : Thread_detach(worker->th);
  Mutex_delete(worker->mut);
  Cond_delete(worker->cv);
  pfree(worker);
}

static void Worker_waitStart(Worker* worker) {
  Mutex_lock(worker->mut);
  while (worker->th == INVALID_THREAD_HANDLE)
    Cond_wait(worker->mut, worker->cv);
  Mutex_unlock(worker->mut);
  LOG_P(LOG_DEBUG, "new %s worker %lu", Worker_typeName(worker->type), Thread_currentId());
}

static void Worker_run(struct Runnable* runnable) {
  Worker* worker = (void*)runnable;
  WorkerType worker_type = worker->type;
  ThreadPoolExecutor* executor = worker->executor;
  Worker_waitStart(worker);
  execAndSignal(worker->initial_pair);
  ExecPair ep;
  while (Worker_getTask(worker, &ep) && ep.task)
    execAndSignal(ep);
  if (!ep.task)
    BlockingQueue_put(worker->executor->task_queue, &ep);
  if (worker->type == WORKER_TYPE_TEMP && WorkerSet_remove(&executor->tmp_ws, worker))
    Worker_close(worker, false);
  LOG_P(LOG_DEBUG, "%s worker %lu exit", Worker_typeName(worker_type), Thread_currentId());
}

bool Worker_init(Worker* worker, ExecPair ep, ThreadPoolExecutor* executor) {
  static void* wr_vptr[] = {Worker_run};
  memset(worker, 0, sizeof(*worker));
  worker->th = INVALID_THREAD_HANDLE;
  worker->super.__vptr = (void*)wr_vptr;
  worker->initial_pair = ep;
  worker->executor = executor;
  if ((worker->mut = Mutex_new()) && (worker->cv = Cond_new()))
    return true;
  Mutex_delete(worker->mut);
  return false;
}

void Worker_destroy(Worker* worker) {
  Mutex_delete(worker->mut);
  Cond_delete(worker->cv);
}
