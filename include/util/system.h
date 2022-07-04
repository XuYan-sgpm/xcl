//
// Created by xuyan on 2022/7/4.
//

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

const static int64_t NANOS_PER_SEC = 1000000000;

int64_t currentMillis();
int64_t nanos();

#ifdef __cplusplus
}
#endif
