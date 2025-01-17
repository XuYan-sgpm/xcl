#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Callable Callable;
struct Callable {
  struct {
    bool (*call)(Callable*);
  }* __vptr;
};

typedef struct {
  Callable super;
  bool (*f)();
} BoolFuncForwarder;

BoolFuncForwarder BoolFuncForwarder_make(bool (*f)());

#ifdef __cplusplus
}
#endif
