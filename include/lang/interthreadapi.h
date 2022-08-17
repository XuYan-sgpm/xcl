//
// Created by xuyan on 2022/7/21.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "lang/localstorage.h"
#include "lang/thread.h"

CLocalStorage*
__Thread_getLocalStorage();

bool
__Thread_setLocalStorage(CLocalStorage* localStorage);

uintptr_t
__Thread_createHandle(void* args);

bool
__Thread_joinFor(uintptr_t handle, int32_t timeout);

bool
__Thread_detach(uintptr_t handle);

bool
__Thread_alive(uintptr_t handle);

unsigned long
__Thread_currentId();

uintptr_t
__Thread_currentHandle();

void
__Thread_yield();

void
__Thread_run(void* args);

#ifdef __cplusplus
}
#endif
