//
// Created by xuyan on 2022/6/20.
//

#include "xcl/lang/CThreadLocal.h"
#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CBaseThreadImpl.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/lang/XclDef.h>

#ifndef _MSC_VER

void
__LocalId_initQueue();

__attribute__((constructor)) static void
__ThreadLocal_initCtx() {
  __LocalId_initQueue();
}

#endif

/**
 * thread local id starts from 1 not 0, because
 * position 0 is token by CThread to store current
 * thread object
 */
static atomic_int_fast32_t __localIdGenerator = 1;

bool
__ThreadLocal_offerId(int32_t id);
bool
__ThreadLocal_pollId(int32_t* id);

static int32_t
__ThreadLocal_newId() {
  return atomic_fetch_add_explicit(
      &__localIdGenerator, 1, memory_order_seq_cst);
}

static int32_t
__ThreadLocal_getId() {
  int32_t id;
  if (!__ThreadLocal_pollId(&id)) {
    id = __ThreadLocal_newId();
  }
  return id;
}

static void
__ThreadLocal_recycleId(long id) {
  __ThreadLocal_offerId(id);
}

static CLocalStorage*
__ThreadLocal_newLocalStorage() {
  CLocalStorage* localStorage = (CLocalStorage*)malloc(sizeof(CLocalStorage));
  if (!localStorage) {
    return NULL;
  }
  memset(localStorage, 0, sizeof(CLocalStorage));
  return localStorage;
}

static CLocalStorage*
__ThreadLocal_checkoutLocalStorage() {
  CLocalStorage* localStorage = __Thread_getLocalStorage();
  if (!localStorage) {
    localStorage = __ThreadLocal_newLocalStorage();
    if (localStorage && !__Thread_setLocalStorage(localStorage)) {
      free(localStorage);
      localStorage = NULL;
    }
  }
  return localStorage;
}

static bool
__ThreadLocal_setData(CThreadLocal* local, const void* src, int len) {
  if (local->id < 0) {
    return false;
  }
  CLocalStorage* localStorage = __ThreadLocal_checkoutLocalStorage();
  if (!localStorage) {
    return false;
  }
  return LocalStorage_setTiny(localStorage, local->id, src, len);
}

static void*
__ThreadLocal_getData(CThreadLocal* local) {
  if (local->id < 0) {
    return NULL;
  }
  CLocalStorage* localStorage = __Thread_getLocalStorage();
  if (!localStorage) {
    return NULL;
  }
  return LocalStorage_get(localStorage, local->id);
}

XCL_PUBLIC(CThreadLocal)
Local_make() {
  CThreadLocal result;
  result.id = __ThreadLocal_getId();
  return result;
}
XCL_PUBLIC(void)
Local_discard(CThreadLocal* local) {
  if (local->id != -1) {
    __ThreadLocal_recycleId(local->id);
    local->id = -1;
  }
}
XCL_PUBLIC(bool)
Local_set(CThreadLocal* local, void* ptr) {
  return __ThreadLocal_setData(local, &ptr, sizeof(void*));
}
XCL_PUBLIC(bool)
Local_setChar(CThreadLocal* local, char val) {
  return __ThreadLocal_setData(local, &val, sizeof(char));
}
XCL_PUBLIC(bool)
Local_setU8(CThreadLocal* local, unsigned char val) {
  return __ThreadLocal_setData(local, &val, sizeof(unsigned char));
}
XCL_PUBLIC(bool)
Local_setInt32(CThreadLocal* local, int32_t val) {
  return __ThreadLocal_setData(local, &val, 4);
}
XCL_PUBLIC(bool)
Local_setU32(CThreadLocal* local, uint32_t val) {
  return __ThreadLocal_setData(local, &val, 4);
}
XCL_PUBLIC(bool)
Local_setInt16(CThreadLocal* local, int16_t val) {
  return __ThreadLocal_setData(local, &val, 2);
}
XCL_PUBLIC(bool)
Local_setU16(CThreadLocal* local, uint16_t val) {
  return __ThreadLocal_setData(local, &val, 2);
}
XCL_PUBLIC(bool)
Local_setFloat(CThreadLocal* local, float val) {
  return __ThreadLocal_setData(local, &val, 4);
}
XCL_PUBLIC(bool)
Local_setInt64(CThreadLocal* local, int64_t val) {
  return __ThreadLocal_setData(local, &val, 8);
}
XCL_PUBLIC(bool)
Local_setU64(CThreadLocal* local, uint64_t val) {
  return __ThreadLocal_setData(local, &val, 8);
}
XCL_PUBLIC(bool)
Local_setDouble(CThreadLocal* local, double val) {
  return __ThreadLocal_setData(local, &val, 8);
}
XCL_PUBLIC(bool)
Local_get(CThreadLocal* local, void** result) {
  void* data = __ThreadLocal_getData(local);
  if (!data) {
    return false;
  }
  if (result) {
    *result = (void*)*(intptr_t*)data;
  }
  return true;
}
XCL_PUBLIC(bool)
Local_getChar(CThreadLocal* local, char* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(char*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getU8(CThreadLocal* local, unsigned char* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(unsigned char*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getInt16(CThreadLocal* local, int16_t* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(int16_t*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getU16(CThreadLocal* local, uint16_t* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(uint16_t*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getInt32(CThreadLocal* local, int32_t* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(int32_t*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getU32(CThreadLocal* local, uint32_t* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(uint32_t*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getFloat(CThreadLocal* local, float* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(float*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getInt64(CThreadLocal* local, int64_t* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(int64_t*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getU64(CThreadLocal* local, uint64_t* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(uint64_t*)data;
  }
  return data;
}
XCL_PUBLIC(bool)
Local_getDouble(CThreadLocal* local, double* result) {
  void* data = __ThreadLocal_getData(local);
  if (data && result) {
    *result = *(double*)data;
  }
  return data;
}
