//
// Created by xuyan on 2022/7/4.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Mutex Mutex;

#include "xcl/lang/xcl_def.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * create a mutex object
 * default is a recursive mutex
 * @return mutex object if succeed, otherwise return NULL
 */
XCL_EXPORT Mutex* XCL_API
Mutex_new();

/**
 * delete a mutex object
 * @param mutex mutex object
 * @return true if delete successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Mutex_delete(Mutex* mutex);

/**
 * lock a mutex
 * @param mutex mutex object
 * @return true if lock successfully
 */
XCL_EXPORT bool XCL_API
Mutex_lock(Mutex* mutex);

/**
 * release the mutex object
 * @param mutex mutex object
 * @return true if unlock successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Mutex_unlock(Mutex* mutex);

/**
 * try lock mutex
 * @param mutex mutex object
 * @return true if acquire mutex object successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Mutex_tryLock(Mutex* mutex);

/**
 * try lock mutex during timeout, return if acquire mutex successfully
 * or timeout expired
 * @param mutex  mutex object
 * @param timeout timeout milliseconds
 * @return true if lock mutex object during timeout, otherwise false
 */
XCL_EXPORT bool XCL_API
Mutex_tryLock2(Mutex* mutex, int32_t timeout);

#ifdef __cplusplus
}
#endif