//
// Created by xuyan on 2022/6/20.
//

#include "lang/CThreadLocal.h"
#include <lang/CLocalStorage.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#if WINDOWS
static volatile int64_t __localId = 0;
#else
static _Atomic(int64_t) __localId = 0;
#endif

bool offerFreeId(int64_t id);
bool hasFreeId();
bool pollFreeId(int64_t *id);

static int64_t __newId() {
  return atomic_fetch_add_explicit(&__localId, 1, memory_order_seq_cst);
}

static int64_t __getId() {
  int64_t id;
  if (!pollFreeId(&id)) {
    id = __newId();
  }
  return id;
}

static void __recycleId(long id) { offerFreeId(id); }

static CLocalStorage *__newLocalStorage() {
  CLocalStorage *localStorage = (CLocalStorage *)malloc(sizeof(CLocalStorage));
  if (!localStorage) {
    return NULL;
  }
  memset(localStorage, 0, sizeof(CLocalStorage));
  return localStorage;
}

static CLocalStorage *__checkoutLocalStorage() {
  CLocalStorage *localStorage = __TL_getLocalStorage();
  if (!localStorage) {
    localStorage = __newLocalStorage();
    if (localStorage && !__TL_setLocalStorage(localStorage)) {
      free(localStorage);
      localStorage = NULL;
    }
  }
  return localStorage;
}

static bool __setLocalData(CThreadLocal *local, const void *src, int len) {
  if (local->id < 0) {
    return false;
  }
  CLocalStorage *localStorage = __checkoutLocalStorage();
  if (!localStorage) {
    return false;
  }
  return LS_SetTiny(localStorage, local->id, src, len);
}

static void *__getLocalData(CThreadLocal *local) {
  if (local->id < 0) {
    return NULL;
  }
  CLocalStorage *localStorage = __TL_getLocalStorage();
  if (!localStorage) {
    return NULL;
  }
  return LS_Get(localStorage, local->id);
}

CThreadLocal makeLocal() {
  CThreadLocal result;
  result.id = __getId();
  return result;
}
void freeLocal(CThreadLocal *local) {
  if (local->id != -1) {
    __recycleId(local->id);
    local->id = -1;
  }
}
bool setLocal(CThreadLocal *local, void *ptr) {
  return __setLocalData(local, &ptr, sizeof(void *));
}
bool setLocalChar(CThreadLocal *local, char val) {
  return __setLocalData(local, &val, sizeof(char));
}
bool setLocalU8(CThreadLocal *local, unsigned char val) {
  return __setLocalData(local, &val, sizeof(unsigned char));
}
bool setLocalInt32(CThreadLocal *local, int32_t val) {
  return __setLocalData(local, &val, 4);
}
bool setLocalU32(CThreadLocal *local, uint32_t val) {
  return __setLocalData(local, &val, 4);
}
bool setLocalInt16(CThreadLocal *local, int16_t val) {
  return __setLocalData(local, &val, 2);
}
bool setLocalU16(CThreadLocal *local, uint16_t val) {
  return __setLocalData(local, &val, 2);
}
bool setLocalFloat(CThreadLocal *local, float val) {
  return __setLocalData(local, &val, 4);
}
bool setLocalInt64(CThreadLocal *local, int64_t val) {
  return __setLocalData(local, &val, 8);
}
bool setLocalU64(CThreadLocal *local, uint64_t val) {
  return __setLocalData(local, &val, 8);
}
bool setLocalDouble(CThreadLocal *local, double val) {
  return __setLocalData(local, &val, 8);
}
bool getLocal(CThreadLocal *local, void **result) {
  void *data = __getLocalData(local);
  if (!data) {
    return false;
  }
  if (result) {
    *result = (void *)*(intptr_t *)data;
  }
  return true;
}
bool getLocalChar(CThreadLocal *local, char *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(char *)data;
  }
  return data;
}
bool getLocalU8(CThreadLocal *local, unsigned char *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(unsigned char *)data;
  }
  return data;
}
bool getLocalInt16(CThreadLocal *local, int16_t *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(int16_t *)data;
  }
  return data;
}
bool getLocalU16(CThreadLocal *local, uint16_t *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(uint16_t *)data;
  }
  return data;
}
bool getLocalInt32(CThreadLocal *local, int32_t *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(int32_t *)data;
  }
  return data;
}
bool getLocalU32(CThreadLocal *local, uint32_t *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(uint32_t *)data;
  }
  return data;
}
bool getLocalFloat(CThreadLocal *local, float *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(float *)data;
  }
  return data;
}
bool getLocalInt64(CThreadLocal *local, int64_t *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(int64_t *)data;
  }
  return data;
}
bool getLocalU64(CThreadLocal *local, uint64_t *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(uint64_t *)data;
  }
  return data;
}
bool getLocalDouble(CThreadLocal *local, double *result) {
  void *data = __getLocalData(local);
  if (data && result) {
    *result = *(double *)data;
  }
  return data;
}
