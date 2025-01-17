#pragma once

#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Releasable Releasable;
struct _Releasable {
  struct {
    void (*release)(Releasable*);
  }* vt;
};
inline always_inline void release(Releasable* obj) {
  if (obj->vt->release)
    obj->vt->release(obj);
}

#ifdef __cplusplus
}
#endif
