//
// Created by xuyan on 2022/7/4.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

const static int32_t LOG2_QUANTUM = 4;

const static int32_t PAGE_SIZE = 1 << 13;

const static int32_t CHUNK_SIZE = 1 << 24;

const static int32_t PAGE_SHIFTS = 13;

const static int32_t LOG2_GROUP_SIZE = 2;

const static int32_t SUB_PAGE_THRESHOLD = 28 << 10;

#ifdef __cplusplus
}
#endif
