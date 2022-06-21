//
// Created by xuyan on 2022/6/20.
//

#include "lang/CThreadLocal.h"
#include <lang/CLocalStorage.h>
#include <stdatomic.h>

#ifdef STATIC_LIB

#if GNUC || CLANG
#define THREAD_LOCAL __thread
#elif MSVC
#define THREAD_LOCAL __declspec(thread)
#else
#error "unsupported compiler"
#endif

typedef enum { CACHE = 0, BUFFER, ELEMENT } Category;

static THREAD_LOCAL CLocalStorage *__localStorageList[3] = {NULL};
static atomic_long __localIdList[3] = {0};

static long __newId(Category category) {
  return atomic_fetch_add_explicit(
             &__localIdList[category], 1, memory_order_seq_cst) +
         1;
}

#endif
