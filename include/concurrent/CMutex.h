//
// Created by xuyan on 2022/7/4.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * create a mutex object
 * default is a recursive mutex
 * @return mutex object if succeed, otherwise return NULL
 */
void *Mutex_new();

/**
 * delete a mutex object
 * @param mutex mutex object
 */
void Mutex_delete(void *mutex);

/**
 * lock a mutex
 * @param mutex mutex object
 * @return true if lock successfully
 */
bool Mutex_lock(void *mutex);

/**
 * release the mutex object
 * @param mutex mutex object
 */
void Mutex_unlock(void *mutex);

/**
 * try lock mutex
 * @param mutex mutex object
 * @return true if acquire mutex object successfully, otherwise false
 */
bool Mutex_tryLock(void *mutex);

/**
 * try lock mutex during millis, return if acquire mutex successfully
 * or timeout expired
 * @param mutex  mutex object
 * @param millis timeout milliseconds
 * @return true if lock mutex object during millis, otherwise false
 */
bool Mutex_tryLock2(void *mutex, int32_t millis);

/**
 * get mutex context data
 * @param mutex mutex object
 * @return mutex context data if available, otherwise false
 */
void *Mutex_ctx(void *mutex);

#ifdef __cplusplus
}
#endif