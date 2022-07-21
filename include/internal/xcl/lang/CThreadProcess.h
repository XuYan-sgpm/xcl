//
// Created by xuyan on 2022/7/21.
//

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "xcl/lang/CThreadState.h"

typedef struct {
    CThreadState tabs[7];
    int32_t cursor;
} CThreadProcess;

#ifdef __cplusplus
extern "C" {
#endif

void ThreadProcess_init(CThreadProcess* process);

void ThreadProcess_push(CThreadProcess* process, CThreadState state);

bool ThreadProcess_check(CThreadProcess* process,
                         bool exist,
                         CThreadState state);

bool ThreadProcess_check2(CThreadProcess* process, bool exist, int32_t n, ...);

CThreadState ThreadProcess_current(CThreadProcess* process);

#ifdef __cplusplus
}
#endif
