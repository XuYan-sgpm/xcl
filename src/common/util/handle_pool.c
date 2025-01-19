#include <internal/util/handle_pool.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/system.h>
#include <xcl/util/list.h>
#include <xcl/util/log.h>
#include <xcl/util/page.h>

void HandleEntry_init(HandleEntry* e, void* addr, void (*destructor)(void*)) {
  e->addr = addr;
  e->destructor = destructor;
  e->refs = 1;
}

void HandleEntry_deleteObject(HandleEntry* e) {
  if (e->destructor)
    e->destructor(e->addr);
  e->addr = NULL;
  e->destructor = NULL;
  e->name = NULL;
}

bool HandleEntry_close(HandleEntry* e) {
  return --e->refs > 0 ? false : (HandleEntry_deleteObject(e), true);
}

typedef struct HandleSpan HandleSpan;
struct HandleSpan {
  ListNode super;
  HandleEntry* entry_list;
  int usage;
};

static int span_entry_size = -1;
static int span_entry_offset = -1;

void HandlePool_globalInit() {
  int page_size = Page_size();
  LOG_ASSERT((page_size & (page_size - 1)) == 0);
  span_entry_size = (page_size - sizeof(HandleSpan)) / sizeof(HandleEntry);
  span_entry_offset = (page_size - span_entry_size * sizeof(HandleEntry));
  LOG_P(LOG_DEBUG, "span entry size: %d, offset: %d", span_entry_size, span_entry_offset);
}

static void HandleSpan_init(HandleSpan* span) {
  memset(span, 0, Page_size());
  HandleEntry* e = (void*)((char*)span + Page_size());
  HandleEntry* next = NULL;
  for (int i = 0; i < span_entry_size; i++) {
    (--e)->next = next;
    next = e;
  }
  span->entry_list = e;
}

static HandleSpan* HandleSpan_new() {
  HandleSpan* span = Page_alloc(NULL, 1);
  if (span) {
    LOG_ASSERT(((uintptr_t)span & (Page_size() - 1)) == 0);
    HandleSpan_init(span);
  }
  return span;
}

static void HandleSpan_free(HandleSpan* span) { Page_free(span, 1); }

static HandleEntry* HandleSpan_alloc(HandleSpan* span) {
  HandleEntry* e = NULL;
  if (span->entry_list) {
    e = span->entry_list;
    span->entry_list = e->next;
    span->usage++;
  }
  return e;
}

static void HandleSpan_recycle(HandleSpan* span, HandleEntry* e) {
  e->next = span->entry_list;
  span->entry_list = e;
  span->usage--;
}

#define POOL_SHIFT 15
#define POOL_SIZE (1 << POOL_SHIFT)
#define POOL_MASK (POOL_SIZE - 1)

struct HandlePool {
  List recycled;
  List avail;
};

HandlePool* HandlePool_new() {
  HandlePool* pool = palloc(sizeof(HandlePool));
  if (pool) {
    List_init(&pool->recycled);
    List_init(&pool->avail);
  }
  return pool;
}

void HandlePool_free(HandlePool* pool) {
  if (pool) {
    while (!List_empty(&pool->recycled))
      HandleSpan_free((void*)List_popFront(&pool->recycled));
    pfree(pool);
  }
}

static HandleSpan* HandlePool_allocSpan(HandlePool* pool) {
  return !List_empty(&pool->recycled) ? (void*)List_popFront(&pool->recycled) : HandleSpan_new();
}
static void HandlePool_recycleSpan(HandlePool* pool, HandleSpan* span) {
  List_erase(&pool->avail, &span->super);
  if (pool->recycled.size < POOL_SIZE)
    List_addBack(&pool->recycled, &span->super);
  else
    HandleSpan_free(span);
}

HandleEntry* HandlePool_alloc(HandlePool* pool) {
  HandleSpan* span;
  if (List_empty(&pool->avail)) {
    if (!(span = HandlePool_allocSpan(pool)))
      return NULL;
    List_addBack(&pool->avail, &span->super);
  } else {
    span = (void*)List_first(&pool->avail);
  }
  HandleEntry* e = HandleSpan_alloc(span);
  LOG_ASSERT(e != NULL);
  if (!span->entry_list) {
    LOG_ASSERT(span->usage == span_entry_size);
    List_popFront(&pool->avail);
  }
  return e;
}

void HandlePool_recycle(HandlePool* pool, HandleEntry* e) {
  if (e) {
    HandleSpan* span = (void*)((uintptr_t)e & ~(Page_size() - 1));
    HandleSpan_recycle(span, e);
    if (span->usage == 0)
      HandlePool_recycleSpan(pool, span);
    else if (span->usage == span_entry_size - 1)
      List_addBack(&pool->avail, &span->super);
  }
}
