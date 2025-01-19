//
// Created by 14049 on 2024/5/5.
//

#include <internal/concurrent/atomic.h>
#include <string.h>
#include <xcl/concurrent/condition.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/concurrent/util/linked_blocking_queue.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>

struct Link {
  struct Link* next;
};

static struct Link* Link_new(unsigned value_size, const void* value) {
  struct Link* link;
  if (value && (link = palloc(sizeof(struct Link) + value_size))) {
    link->next = NULL;
    memcpy(link + 1, value, value_size);
    return link;
  }
  return NULL;
}

struct LinkedBlockingQueue {
  BlockingQueue super;
  struct Link *head, *tail;
  void* push_mut;
  void* not_full;
  void* pop_mut;
  void* not_empty;
  volatile int size;
  int value_size;
  unsigned max_size;
};

static void LBQ_signalNotEmpty(struct LinkedBlockingQueue* q) {
  Mutex_lock(q->pop_mut);
  Cond_signal(q->not_empty);
  Mutex_unlock(q->pop_mut);
}
static void LBQ_signalNotFull(struct LinkedBlockingQueue* q) {
  Mutex_lock(q->push_mut);
  Cond_signal(q->not_full);
  Mutex_unlock(q->push_mut);
}

static bool LBQ_offer(BlockingQueue* obj, const void* value) {
  struct LinkedBlockingQueue* q = (void*)obj;
  struct Link* link = NULL;
  if (_load32(&q->size) >= q->max_size || !(link = Link_new(q->value_size, value)))
    return false;
  Mutex_lock(q->push_mut);
  int c = _load32(&q->size);
  if (c >= q->max_size) {
    Mutex_unlock(q->push_mut);
    pfree(link);
    return false;
  }
  q->tail->next = link;
  q->tail = link;
  c = _fetch_add32(&q->size, 1);
  if (c + 1 < q->max_size)
    Cond_signal(q->not_full);
  Mutex_unlock(q->push_mut);
  if (!c)
    LBQ_signalNotEmpty(q);
  return true;
}

static bool LBQ_put(BlockingQueue* obj, const void* value) {
  struct LinkedBlockingQueue* q = (void*)obj;
  struct Link* link = Link_new(q->value_size, value);
  if (!link)
    return false;
  Mutex_lock(q->push_mut);
  while (_load32(&q->size) >= q->max_size)
    Cond_wait(q->push_mut, q->not_full);
  q->tail->next = link;
  q->tail = link;
  int c = _fetch_add32(&q->size, 1);
  if (c + 1 < q->max_size)
    Cond_signal(q->not_full);
  Mutex_unlock(q->push_mut);
  if (!c)
    LBQ_signalNotEmpty(q);
  return true;
}

static bool LBQ_put2(BlockingQueue* obj, const void* value, int timeout) {
  if (!timeout)
    return LBQ_offer(obj, value);
  struct LinkedBlockingQueue* q = (void*)obj;
  struct Link* link = Link_new(q->value_size, value);
  if (!link)
    return false;
  Mutex_lock(q->push_mut);
  int64_t wait_limit = timeout < 0 ? 0xffffffff : timeout;
  int64_t deadline = nanos() + wait_limit * 1000000;
  int64_t now;
  while (_load32(&q->size) >= q->max_size && (now = nanos()) < deadline)
    Cond_waitFor(q->push_mut, q->not_full, (deadline - now) / 1000000);
  if (_load32(&q->size) >= q->max_size) {
    Mutex_unlock(q->push_mut);
    pfree(link);
    return false;
  }
  q->tail->next = link;
  q->tail = link;
  int c = _fetch_add32(&q->size, 1);
  if (c + 1 < q->max_size)
    Cond_signal(q->not_full);
  Mutex_unlock(q->push_mut);
  if (!c)
    LBQ_signalNotEmpty(q);
  return true;
}

static bool LBQ_poll(BlockingQueue* obj, void* value) {
  struct LinkedBlockingQueue* q = (void*)obj;
  struct Link* freed = NULL;
  Mutex_lock(q->pop_mut);
  int c = _load32(&q->size);
  if (c > 0) {
    freed = q->head;
    if (value)
      memcpy(value, freed->next + 1, q->value_size);
    q->head = freed->next;
    c = _fetch_sub32(&q->size, 1);
    if (c > 1)
      Cond_signal(q->not_empty);
  }
  Mutex_unlock(q->pop_mut);
  if (c == q->max_size)
    LBQ_signalNotFull(q);
  if (freed)
    pfree(freed);
  return c > 0;
}

static void LBQ_take(BlockingQueue* obj, void* value) {
  struct LinkedBlockingQueue* q = (void*)obj;
  Mutex_lock(q->pop_mut);
  while (_load32(&q->size) == 0)
    Cond_wait(q->pop_mut, q->not_empty);
  struct Link* freed = q->head;
  if (value)
    memcpy(value, freed->next + 1, q->value_size);
  q->head = freed->next;
  int c = _fetch_sub32(&q->size, 1);
  if (c > 1)
    Cond_signal(q->not_empty);
  Mutex_unlock(q->pop_mut);
  if (c == q->max_size)
    LBQ_signalNotFull(q);
  pfree(freed);
}

static bool LBQ_take2(BlockingQueue* obj, void* value, int timeout) {
  if (!timeout)
    return LBQ_poll(obj, value);
  int c;
  struct LinkedBlockingQueue* q = (void*)obj;
  Mutex_lock(q->pop_mut);
  int64_t wait_limit = timeout < 0 ? 0xffffffff : timeout;
  int64_t deadline = nanos() + wait_limit * 1000000;
  int64_t now;
  while ((c = _load32(&q->size)) == 0 && (now = nanos()) < deadline)
    Cond_waitFor(q->pop_mut, q->not_empty, (deadline - now) / 1000000);
  if (c == 0) {
    Mutex_unlock(q->pop_mut);
    return false;
  }
  struct Link* freed = q->head;
  if (value)
    memcpy(value, freed->next + 1, q->value_size);
  q->head = freed->next;
  c = _fetch_sub32(&q->size, 1);
  if (c > 1)
    Cond_signal(q->not_empty);
  Mutex_unlock(q->pop_mut);
  if (c == q->max_size)
    LBQ_signalNotFull(q);
  pfree(freed);
  return true;
}

static void* LBQ_peek(BlockingQueue* obj) {
  void* value = NULL;
  struct LinkedBlockingQueue* q = (void*)obj;
  Mutex_lock(q->pop_mut);
  if (_load32(&q->size))
    value = q->head->next + 1;
  Mutex_unlock(q->pop_mut);
  return value;
}

static void LBQ_clear(BlockingQueue* obj) {
  while (LBQ_poll(obj, NULL)) {
  }
}

static int LBQ_size(BlockingQueue* obj) {
  return _load32(&((struct LinkedBlockingQueue*)obj)->size);
}

static bool LBQ_empty(BlockingQueue* obj) { return LBQ_size(obj) == 0; }

BlockingQueue* LinkedBlockingQueue_new(int value_size, int max_size) {
  static void* lbq_vptr[] = {LBQ_offer, LBQ_put,   LBQ_put2, LBQ_poll, LBQ_take,
                             LBQ_take2, LBQ_clear, LBQ_peek, LBQ_size, LBQ_empty};
  struct LinkedBlockingQueue* q = palign(sizeof(struct LinkedBlockingQueue), PTR_SIZE);
  if (q) {
    memset(q, 0, sizeof(*q));
    if ((q->head = q->tail = palloc(sizeof(struct Link))) && (q->not_empty = Cond_new()) &&
        ((q->not_full = Cond_new())) && (q->push_mut = Mutex_new()) && (q->pop_mut = Mutex_new())) {
      q->max_size = max_size;
      q->head->next = NULL;
      q->value_size = value_size;
      _store32(&q->size, 0);
      q->super.__vptr = (void*)lbq_vptr;
      return &q->super;
    }
    pfree(q->head);
    Cond_delete(q->not_empty);
    Cond_delete(q->not_full);
    Mutex_delete(q->push_mut);
    Mutex_delete(q->pop_mut);
    pfreea(q);
  }
  return NULL;
}

void LinkedBlockingQueue_free(BlockingQueue* bq) {
  struct LinkedBlockingQueue* q = (void*)bq;
  if (q) {
    LBQ_clear(bq);
    pfree(q->head);
    Cond_delete(q->not_empty);
    Cond_delete(q->not_full);
    Mutex_delete(q->push_mut);
    Mutex_delete(q->pop_mut);
    pfreea(q);
  }
}
