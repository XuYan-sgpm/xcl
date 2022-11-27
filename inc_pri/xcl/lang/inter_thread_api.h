//
// Created by xuyan on 2022/7/21.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "xcl/lang/local_storage.h"
#include "xcl/lang/thread.h"

LocalStorage*
__Thread_getLocalStorage();

bool
__Thread_setLocalStorage(LocalStorage* localStorage);

#ifdef __cplusplus
}
#endif
