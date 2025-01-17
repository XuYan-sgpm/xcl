//
// Created by 徐琰 on 2023/10/19.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * new a concurrent linked queue
 * @param limit max node allocated
 * @return queue object
 */
void* CLQ_new();

/**
 * delete concurrent linked queue
 * @param q concurrent linked queue
 */
void CLQ_delete(void* q);

/**
 * offer a object to queue
 * @param q queue
 * @param a object, not large than 64-bit integer
 * @return true if offer successfully, otherwise false
 */
bool CLQ_offer(void* q, uint64_t a);

/**
 * poll object from queue
 * @param q queue
 * @param val pointer hold object
 * @return true if queue is not empty
 */
bool CLQ_poll(void* q, uint64_t* val);

/**
 * get queue size_
 * @param q queue
 * @return size_ of concurrent linked queue
 */
unsigned CLQ_size(void* q);

/**
 * check whether queue is empty
 * @param q queue
 * @return true if queue is empty
 */
bool CLQ_empty(void* q);

/**
 * clear queue
 * @param q queue
 */
void CLQ_clear(void* q);

#ifdef __cplusplus
};
#endif
