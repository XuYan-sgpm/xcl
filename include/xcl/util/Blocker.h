//
// Created by xy on 7/11/22.
//

/**
 * simple blocker component
 * be careful, blocker may be returned
 * during wait even if no signal received
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/xcl_def.h"
#include "xcl/concurrent/mutex.h"
#include <stdbool.h>

typedef struct Blocker Blocker;

/**
 * new a blocker object
 * @return blocker object if create successfully, otherwise false
 */
XCL_EXPORT Blocker* XCL_API
Blocker_new();

/**
 * new a blocker object with a mutex object
 * @param mutex mutex object (Mutex)
 * @return blocker object if create successfully, otherwise false
 */
XCL_EXPORT Blocker* XCL_API
Blocker_new2(Mutex* mutex);

/**
 * delete the blocker object
 * this method is not thread safe, should called
 * when blocker is no longer use. call Blocker_delete
 * on using blocker is undefined behaviour
 * @param blocker blocker object
 * @return true if blocker is not in waiting state,
 * otherwise false
 */
XCL_EXPORT bool XCL_API
Blocker_delete(Blocker* blocker);

/**
 * wait blocker util signal received
 * @param blocker blocker object
 * @return blocker wait successfully
 */
XCL_EXPORT bool XCL_API
Blocker_wait(Blocker* blocker);

/**
 * send a signal if blocker is in waiting state
 * if blocker is not waiting, next wait call
 * will be passed
 * @param blocker blocker object
 * @return blocker notify successfully
 */
XCL_EXPORT bool XCL_API
Blocker_cancel(Blocker* blocker);

/**
 * wake all waiting blocker
 * @param blocker blocker object
 * @return notify all waiting blocker successfully
 */
XCL_EXPORT bool XCL_API
Blocker_wakeAll(Blocker* blocker);

#ifdef __cplusplus
}
#endif
