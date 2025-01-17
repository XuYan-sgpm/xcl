#include <internal/concurrent/util/worker_set.h>
#include <string.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/util/comparators.h>
#include <xcl/util/log.h>
#include <xcl/util/set.h>

static void WorkerSet_clear(WorkerSet* ws) {
  for (;;) {
    Worker* worker = NULL;
    Mutex_lock(ws->mut);
    TreeIter it = Set_begin(ws->set);
    if (TreeIter_valid(it)) {
      worker = *(Worker**)TreeIter_value(it);
      Set_removeIter(ws->set, it);
    }
    Mutex_unlock(ws->mut);
    if (!worker)
      break;
    Worker_close(worker, true);
  }
}

bool WorkerSet_init(WorkerSet* ws, int limit) {
  ws->limit = limit;
  if ((ws->mut = Mutex_new()) && (ws->set = Set_new(sizeof(void*), pcomp())))
    return true;
  Mutex_delete(ws->mut);
  memset(ws, 0, sizeof(*ws));
  return false;
}

void WorkerSet_destroy(WorkerSet* ws) {
  if (ws) {
    WorkerSet_clear(ws);
    Set_free(ws->set);
    Mutex_delete(ws->mut);
    memset(ws, 0, sizeof(*ws));
  }
}

bool WorkerSet_add(WorkerSet* ws, Worker* worker) {
  bool added = false;
  Mutex_lock(ws->mut);
  if (ws->limit >= 0 && Set_size(ws->set) < ws->limit)
    added = TreeIter_valid(Set_add(ws->set, true, &worker));
  Mutex_unlock(ws->mut);
  return added;
}

bool WorkerSet_remove(WorkerSet* ws, Worker* worker) {
  bool removed = false;
  Mutex_lock(ws->mut);
  removed = !Set_empty(ws->set) && Set_remove(ws->set, &worker);
  Mutex_unlock(ws->mut);
  return removed;
}
