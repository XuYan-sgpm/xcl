//
// Created by 徐琰 on 2023/10/13.
//

#pragma once

#include <stdbool.h>
#include <xcl/lang/define.h>
#include <xcl/lang/runnable.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_THREAD_HANDLE (~(uintptr_t)0)

uintptr_t Thread_create(struct Runnable* runnable);
bool Thread_join(uintptr_t thread);
int Thread_join2(uintptr_t thread, unsigned timeout);
bool Thread_detach(uintptr_t thread);
uintptr_t Thread_current(void);
unsigned long Thread_currentId();
bool Thread_atExit(struct Runnable* runnable);
int Thread_alive(uintptr_t thread);

#ifdef __cplusplus
}
#endif
