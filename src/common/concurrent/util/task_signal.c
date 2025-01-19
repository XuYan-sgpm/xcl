#include <string.h>
#include <xcl/concurrent/condition.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/concurrent/util/task_signal.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/util/log.h>

struct TaskSignal {
  void* mut;
  void* cv;
  int state;
  int refs;
};

TaskSignal* TaskSignal_new() {
  TaskSignal* sig = palloc(sizeof(TaskSignal));
  if (sig) {
    if ((sig->mut = Mutex_new()) && (sig->cv = Cond_new())) {
      sig->state = TASK_STATE_INIT;
      sig->refs = 1;
      return sig;
    }
    Mutex_delete(sig->mut);
    pfree(sig);
  }
  return sig;
}

void TaskSignal_free0(TaskSignal* sig) {
  if (sig) {
    Mutex_delete(sig->mut);
    Cond_delete(sig->cv);
    pfree(sig);
  }
}

void TaskSignal_free(TaskSignal* sig) {
  if (sig) {
    Mutex_lock(sig->mut);
    bool no_ref = --sig->refs == 0;
    Mutex_unlock(sig->mut);
    if (no_ref)
      TaskSignal_free0(sig);
  }
}

bool TaskSignal_trigger(TaskSignal* sig, int state) {
  Mutex_lock(sig->mut);
  if (sig->state > TASK_STATE_EXECUTING || state < sig->state) {
    Mutex_unlock(sig->mut);
    return false;
  }
  if (sig->state == TASK_STATE_EXECUTING && state > sig->state)
    Cond_broadcast(sig->cv);
  sig->state = state;
  Mutex_unlock(sig->mut);
  return true;
}

TaskSignal* TaskSignal_ref(TaskSignal* sig) {
  Mutex_lock(sig->mut);
  if (sig->refs > 0)
    ++sig->refs;
  else
    sig = NULL;
  Mutex_unlock(sig->mut);
  return sig;
}

int TaskSignal_state(TaskSignal* sig) {
  Mutex_lock(sig->mut);
  int state = sig->state;
  Mutex_unlock(sig->mut);
  return state;
}

int TaskSignal_wait(TaskSignal* sig, int timeout) {
  Mutex_lock(sig->mut);
  int state;
  if ((state = sig->state) > TASK_STATE_EXECUTING) {
    Mutex_unlock(sig->mut);
    return state;
  }
  if (state != TASK_STATE_EXECUTING)
    state = sig->state = TASK_STATE_EXECUTING;
  if (timeout == 0) {
    Mutex_unlock(sig->mut);
    return state;
  }
  int64_t now;
  int64_t deadline = currentMillis() + (timeout < 0 ? 0xffffffff : timeout);
  while ((state = sig->state) == TASK_STATE_EXECUTING && (now = currentMillis()) < deadline)
    Cond_waitFor(sig->mut, sig->cv, deadline - now);
  Mutex_unlock(sig->mut);
  return state;
}
