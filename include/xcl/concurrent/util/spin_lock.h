//
// Created by 徐琰 on 2023/10/21.
//

#pragma once

#include <stdbool.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  _aligned(4) volatile int handle;
} SpinLock;

void SpinLock_init(SpinLock* spin_lock);
bool SpinLock_try(SpinLock* spin_lock);
void SpinLock_lock(SpinLock* spin_lock);
void SpinLock_unlock(SpinLock* spin_lock);

#ifdef __cplusplus
};
#endif
