//
// Created by xuyan on 2022/7/21.
//

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CThread.h"

#ifdef __cplusplus
extern "C" {
#endif

CLocalStorage* __Thread_getLocalStorage();

bool __Thread_setLocalStorage(CLocalStorage* localStorage);

bool __Thread_createHandle(void* args, CThread* thread);

bool __Thread_wait(uintptr_t handle, int32_t timeout);

bool __Thread_detach(uintptr_t handle);

bool __Thread_alive(uintptr_t handle);

unsigned long __Thread_currentId();

uintptr_t __Thread_currentHandle();

bool __Thread_closeHandle(uintptr_t handle);

void __Thread_run(void* args);

#ifdef __cplusplus
}
#endif
