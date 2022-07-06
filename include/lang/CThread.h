//
// Created by xuyan on 2022/7/6.
//

#pragma once

#include <lang/XclDef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CThread_st CThread;

/**
 * new a thread object
 * @param suspend if thread suspend when created
 * @return thread object if successfully, otherwise false
 */
XCL_PUBLIC CThread* XCL_API Thread_new(bool suspend);

/**
 * get current thread object
 * @return thread object current running
 */
XCL_PUBLIC CThread* XCL_API Thread_current();

#ifdef __cplusplus
}
#endif
