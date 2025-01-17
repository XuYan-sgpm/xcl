//
// Created by xuyan on 2022/7/5.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/lang/define.h>

/**
 * new condition variable
 * @return condition variable if successfully, otherwise NULL
 */
void* Cond_new();

/**
 * delete condition variable
 * @param cond condition variable
 */
bool Cond_delete(void* cond);

/**
 * wait signal util Cond_signal called
 * @param cond condition variable
 */
bool Cond_wait(void* mutex, void* cond);

/**
 * wait signal on the condition during millis
 * this function may returned even if no signal
 * send to current thread
 * @param cond condition variable
 * @param millis timeout milliseconds
 * @return true if condition variable signaled, otherwise false
 */
bool Cond_waitFor(void* mutex, void* cond, unsigned millis);

/**
 * signal random thread wait on cond
 * @param cond condition variable
 */
bool Cond_signal(void* cond);

/**
 * signal all threads_ wait on cond
 * @param cond condition variable
 */
bool Cond_broadcast(void* cond);

#ifdef __cplusplus
}
#endif
