//
// Created by 14049 on 2024/5/10.
//

#pragma once

#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Runnable {
  struct {
    void (*run)(struct Runnable*);
  }* __vptr;
};
inline always_inline void run(struct Runnable* obj) { obj->__vptr->run(obj); }

#ifdef __cplusplus
}
#endif
