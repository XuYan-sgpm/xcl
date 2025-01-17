#pragma once

#include <stdbool.h>
#include <xcl/lang/define.h>
#include <xcl/util/callable.h>
#include <xcl/util/factory.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  volatile char value;
} LazyFlag;

bool Lazy_invoke(LazyFlag* flag, Callable* callable);

typedef struct {
  _aligned(PTR_SIZE) void* volatile addr;
} LazyObject;

void* Lazy_get(LazyObject* obj, Factory* factory);

#ifdef __cplusplus
}
#endif