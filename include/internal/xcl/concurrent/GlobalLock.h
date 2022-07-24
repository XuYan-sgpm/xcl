//
// Created by 徐琰 on 2022/7/24.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

bool __acquireGlobalLock();

void __releaseGlobalLock();

#ifdef __cplusplus
}
#endif
