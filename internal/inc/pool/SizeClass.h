//
// Created by 徐琰 on 2022/6/26.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>

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

/**
 * get size class items
 * @return size tab count
 */
int32_t SizeClass_size();

/**
 * get one tab from size class
 * @param idx tab index
 * @param tab output
 * @return get tab successfully if true, otherwise false
 */
bool SizeClass_get(int32_t idx, int32_t out[6]);

/**
 * normalize size
 * @param size original size
 * @return size normalized by size class
 */
uint32_t SizeClass_normalize(uint32_t size);

/**
 * normalize size to pages
 * @param size original size
 * @return normalized pages
 */
uint32_t SizeClass_size2pages(uint32_t size);

#ifdef __cplusplus
}
#endif
