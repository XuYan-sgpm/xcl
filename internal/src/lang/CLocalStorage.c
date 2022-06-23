//
// Created by xuyan on 2022/6/21.
//

#include <lang/CLocalStorage.h>
#include <stdlib.h>
#include <string.h>

const static int __initialLocalStorageSize = 8;

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

static CLocalStorage *__checkoutLocalStorageMem(CLocalStorage *localStorage,
                                                int idx) {
  if (!localStorage->cap) {
    int cap =
        __initialLocalStorageSize > idx ? __initialLocalStorageSize : idx + 1;
    if (!__reserveLocalStorage(localStorage, cap)) {
      return NULL;
    }
  } else {
    if (!__reserveLocalStorage(localStorage, idx + 1)) {
      return NULL;
    }
  }
  return localStorage;
}
void *LS_Get(CLocalStorage *localStorage, int idx) {
  if (!__checkoutLocalStorageMem(localStorage, idx)) {
    return NULL;
  }
  return localStorage->blocks[idx].data;
}
void LS_Free(CLocalStorage *localStorage) {
  if (localStorage->blocks) {
    free(localStorage->blocks);
    memset(localStorage, 0, sizeof(CLocalStorage));
  }
}
bool LS_SetPtr(CLocalStorage *localStorage, int idx, intptr_t ptr) {
  localStorage = __checkoutLocalStorageMem(localStorage, idx);
  if (!localStorage) {
    return false;
  }
  void *data = LS_Get(localStorage, idx);
  *(intptr_t *)data = ptr;
  return true;
}
bool LS_SetTiny(CLocalStorage *localStorage,
                int idx,
                const void *src,
                int len) {
  localStorage = __checkoutLocalStorageMem(localStorage, idx);
  if (!localStorage) {
    return false;
  }
  void *data = localStorage->blocks[idx].data;
  memcpy(data, src, len);
  return true;
}
