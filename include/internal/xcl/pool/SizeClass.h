//
// Created by 徐琰 on 2022/6/26.
//

#pragma once

#include <xcl/lang/XclDef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * initialize size class
 * @return true if successfully, otherwise false
 */
XCL_PUBLIC(bool)
SizeClass_initialize();

/**
 * release size class resource
 */
XCL_PUBLIC(void)
SizeClass_finalize();

/**
 * get size class items
 * @return size tab count
 */
XCL_PUBLIC(int32_t)
SizeClass_size();

/**
 * get one tab from size class
 * @param idx tab index
 * @param tab output
 * @return get tab successfully if true, otherwise false
 */
XCL_PUBLIC(bool)
SizeClass_get(int32_t idx, int32_t out[6]);

/**
 * normalize size
 * @param size original size
 * @return size normalized by size class
 */
XCL_PUBLIC(uint32_t)
SizeClass_normalize(uint32_t size);

/**
 * normalize size to pages
 * @param size original size
 * @return normalized pages
 */
XCL_PUBLIC(uint32_t)
SizeClass_size2pages(uint32_t size);

#ifdef __cplusplus
}
#endif
