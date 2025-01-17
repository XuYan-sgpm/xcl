//
// Created by 徐琰 on 2023/10/22.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _INVALID_QUEUE_ELEM (~0ull)

/**
 * new concurrent ring queue
 * @param cap max element of queue
 * @return queue
 */
void* CRQ_new(unsigned cap);

/**
 * delete concurrent ring queue
 * @param q queue
 */
void CRQ_delete(void* q);

/**
 * offer element to queue
 * @param q queue
 * @param a element, can not be _INVALID_QUEUE_ELEM
 * @return true if offer successfully, otherwise false
 */
bool CRQ_offer(void* q, uint64_t a);

/**
 * poll element from queue
 * @param q queue
 * @param val pointer hold queue element
 * @return true if queue is not empty, otherwise false
 */
bool CRQ_poll(void* q, uint64_t* val);

/**
 * get queue size_
 * @param q queue
 * @return queue size_
 */
unsigned CRQ_size(void* q);

/**
 * check whether queue is empty
 * @param q queue
 * @return true if queue is empty, otherwise false
 */
bool CRQ_empty(void* q);

/**
 * clear queue
 * @param q queue
 */
void CRQ_clear(void* q);

#ifdef __cplusplus
};
#endif
