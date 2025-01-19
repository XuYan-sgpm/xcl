#include <xcl/util/callable.h>

static bool BoolFuncForwarder_call(Callable* callable) {
  return ((BoolFuncForwarder*)callable)->f();
}
BoolFuncForwarder BoolFuncForwarder_make(bool (*f)()) {
  static const void* const bool_func_forwarder_vptr[] = {BoolFuncForwarder_call};
  BoolFuncForwarder bool_func_forwarder = {.f = f, .super.__vptr = (void*)bool_func_forwarder_vptr};
  return bool_func_forwarder;
}