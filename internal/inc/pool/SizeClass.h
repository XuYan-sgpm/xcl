//
// Created by 徐琰 on 2022/6/26.
//

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * initialize size class
 * @return true if successfully, otherwise false
 */
bool SizeClass_initialize();

/**
 * release size class resource
 */
void SizeClass_finalize();

#ifdef __cplusplus
}
#endif
