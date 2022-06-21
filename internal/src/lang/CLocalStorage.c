//
// Created by xuyan on 2022/6/21.
//

#ifdef STATIC_LIB

#include <lang/CLocalStorage.h>
//#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <lang/Bytes.h>
#include <lang/platform.h>

// static atomic_long __threadLocalIdList[3] = {0};
//
// static long __nextId(Category category) {
//  return atomic_fetch_add_explicit(
//             &__threadLocalIdList[category], 1, memory_order_seq_cst) +
//         1;
//}
//
// static __thread CLocalStorage *__threadLocalStorageList[3] = {NULL};
const static int __initialLocalStorageSize = 8;

// CLocalStorage *getLocalStorage(Category category) {
//  return __threadLocalStorageList[category];
//}

static inline int __grow(CLocalStorage *localStorage, int n) {
  int32_t cap = localStorage->cap;
  return cap + (cap > n - cap ? cap : n - cap);
}

/*
 * reserve n blocks for passing local storage
 */
static bool __reserveLocalStorage(CLocalStorage *localStorage, int n) {
  if (n > localStorage->cap) {
    int32_t newCap = __grow(localStorage, n);
    Block *newBlocks =
        (Block *)realloc(localStorage->blocks, newCap * sizeof(Block));
    if (!newBlocks) {
      return false;
    }
    localStorage->blocks = newBlocks;
    localStorage->cap = newCap;
  }
  return true;
}

CLocalStorage *__newLocalStorage() {
  CLocalStorage *localStorage = (CLocalStorage *)malloc(sizeof(CLocalStorage));
  if (!localStorage) {
    return NULL;
  }
  memset(localStorage, 0, sizeof(CLocalStorage));
  //  __threadLocalStorageList[category] = localStorage;
  return localStorage;
}

static CLocalStorage *__checkoutBeforePush(CLocalStorage *localStorage,
                                           int idx) {
  //  CLocalStorage *localStorage = getLocalStorage(category);
  if (!localStorage->cap) {
    localStorage = __newLocalStorage();
    if (!localStorage ||
        !__reserveLocalStorage(localStorage, __initialLocalStorageSize)) {
      return NULL;
    }
  } else {
    if (!__reserveLocalStorage(localStorage, idx + 1)) {
      return NULL;
    }
  }
  return localStorage;
}

static bool
__pushCache(CLocalStorage *localStorage, int idx, const void *src, int len) {
  localStorage = __checkoutBeforePush(localStorage, idx);
  if (!localStorage) {
    return false;
  }
  char *data = localStorage->blocks[idx].data;
  memcpy(data, src, len);
  int2Bytes(len, data + sizeof(void *), false);
  return true;
}

static bool
__pushBuffer(CLocalStorage *localStorage, int idx, const void *src, int len) {
  localStorage = __checkoutBeforePush(localStorage, idx);
  if (!localStorage) {
    return false;
  }
  char *data = localStorage->blocks[idx].data;
#ifdef X64
  LongLong2Bytes((intptr_t)src, data, false);
#else
  int2Bytes((int32_t)src, data, false)
#endif
  int2Bytes(len, data + sizeof(void *), false);
  return true;
}

static bool __pushObj(CLocalStorage *localStorage,
                      int idx,
                      void *obj,
                      void (*destructor)(void *)) {
  localStorage = __checkoutBeforePush(localStorage, idx);
  if (!localStorage) {
    return false;
  }
  char *data = localStorage->blocks[idx].data;
#ifdef X86
  int2Bytes((int32_t)obj, data, false);
  int2Bytes((int32_t)destructor, data + 4, false);
#else
  LongLong2Bytes((int64_t)obj, data, false);
  LongLong2Bytes((int64_t)destructor, data + 8, false);
#endif
  return true;
}

bool setObj(CLocalStorage *localStorage,
            int idx,
            void *obj,
            void (*destructor)(void *)) {
  return __pushObj(localStorage, idx, obj, destructor);
}

bool setTiny(CLocalStorage *localStorage, int idx, const void *src, int len) {
  return __pushCache(localStorage, idx, src, len);
}

bool setBuffer(CLocalStorage *localStorage, int idx, const void *src, int len) {
  return __pushBuffer(localStorage, idx, src, len);
}
Block *get(CLocalStorage *localStorage, int idx) {
  return &localStorage->blocks[idx];
}
void freeStorage(CLocalStorage *localStorage) {
  if (localStorage->blocks) {
    free(localStorage->blocks);
    memset(localStorage, 0, sizeof(CLocalStorage));
  }
}

#endif
