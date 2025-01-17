//
// Created by xuyan on 2022/7/4.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/define.h>

/**
 * create a mutex object
 * default is a recursive mutex
 * @return mutex object if succeed, otherwise return NULL
 */
void* Mutex_new();

/**
 * delete a mutex object
 * @param mutex mutex object
 * @return true if delete successfully, otherwise false
 */
bool Mutex_delete(void* mutex);

/**
 * lock a mutex
 * @param mutex mutex object
 * @return true if lock successfully
 */
bool Mutex_lock(void* mutex);

/**
 * finalize the mutex object
 * @param mutex mutex object
 * @return true if unlock successfully, otherwise false
 */
bool Mutex_unlock(void* mutex);

/**
 * try lock mutex
 * @param mutex mutex object
 * @return true if acquire mutex object successfully, otherwise false
 */
bool Mutex_tryLock(void* mutex);

/**
 * try lock mutex during timeout, return if acquire mutex successfully
 * or timeout expired
 * @param mutex  mutex object
 * @param timeout timeout milliseconds
 * @return true if lock mutex object during timeout, otherwise false
 */
bool Mutex_timedLock(void* mutex, int32_t timeout);

#ifdef __cplusplus
}
#endif