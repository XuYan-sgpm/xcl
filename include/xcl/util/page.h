//
// Created by 徐琰 on 2023/10/23.
//

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * alloc pages
 * @param hint hint address
 * @param pages page count
 * @return page address
 */
void* Page_alloc(void* hint, size_t pages);

/**
 * free pages
 * @param p address
 * @param pages page count
 */
void Page_free(void* p, size_t pages);

/**
 * @brief Get os system page size_
 * @return system page size_
 */
size_t Page_size();

#ifdef __cplusplus
};
#endif
