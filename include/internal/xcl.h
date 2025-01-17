#pragma once

#include <internal/util/handle_pool.h>
#include <internal/util/handle_table.h>
#include <internal/util/log_impl.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void* __xcl_mutex;
extern void* __xcl_cv;
extern unsigned long __xcl_key;
extern LogImpl* __xcl_log;
extern HandleTable* __xcl_handle_table;
extern HandlePool* __xcl_handle_pool;

typedef enum {
  XCL_MUTEX,
  XCL_CV,
  XCL_LOCAL_KEY,
  XCL_LOG,
  XCL_HANDLE_POOL,
  XCL_NH_TABLE,
  MAX_XCL_ITEM
} XclItem;

void Xcl_init(XclItem item);

#ifdef __cplusplus
}
#endif
