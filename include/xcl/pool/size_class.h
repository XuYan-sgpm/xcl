//
// Created by 徐琰 on 2022/6/26.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/lang/xcl_def.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * initialize size class
 * @return true if successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
SizeClass_initialize();

/**
 * release size class resource
 */
XCL_EXPORT void XCL_API
SizeClass_finalize();

/**
 * get size class items
 * @return size tab count
 */
XCL_EXPORT int32_t XCL_API
SizeClass_size();

/**
 * get one tab from size class
 * @param idx tab index
 * @param tab output
 * @return get tab successfully if true, otherwise false
 */
XCL_EXPORT bool XCL_API
SizeClass_get(int32_t idx, int32_t out[6]);

/**
 * normalize size
 * @param size original size
 * @return size normalized by size class
 */
XCL_EXPORT uint32_t XCL_API
SizeClass_normalize(uint32_t size);

/**
 * normalize size to pages
 * @param size original size
 * @return normalized pages
 */
XCL_EXPORT uint32_t XCL_API
SizeClass_size2pages(uint32_t size);

#ifdef __cplusplus
}
#endif
