//
// Created by 徐琰 on 2022/7/24.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool
__GlobalLock_acquire();

void
__GlobalLock_release();

#ifdef __cplusplus
}
#endif
