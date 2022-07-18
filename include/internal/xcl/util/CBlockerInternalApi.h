//
// Created by xy on 7/14/22.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * basic blocker structure
 * need expand for different platforms
 */
struct _CBlocker_st {
    int32_t wait;
    int32_t notify;
    bool externalLock;
};

typedef struct _CBlocker_st CBlocker;

/**
 * functions below are need to implement
 */

/**
 * alloc a blocker object
 * @return blocker object if alloc successfully, otherwise NULL
 */
CBlocker* __Blocker_alloc();

/**
 * dealloc a blocker object
 * @param blocker blocker object
 */
void __Blocker_dealloc(CBlocker* blocker);

/**
 * init a blocker
 * @param blocker blocker object
 * @param mutex CMutex object, used by blocker
 * @return 0 if successfully, otherwise error code
 */
int32_t __Blocker_init(CBlocker* blocker, void* mutex);

/**
 * acquire blocker mutex object
 * @param blocker blocker object
 * @return 0 if successfully, otherwise error code
 */
int32_t __Blocker_acquire(CBlocker* blocker);

/**
 * release blocker mutex object
 * @param blocker blocker object
 * @return 0 if successfully, otherwise error code
 */
int32_t __Blocker_release(CBlocker* blocker);

/**
 * block current thread util signal sent
 * @param blocker blocker object
 * @return 0 if successfully, otherwise error code
 */
int32_t __Blocker_wait(CBlocker* blocker);

/**
 * send a signal to a waiting thread
 * @param blocker blocker object
 * @return 0 if successfully, otherwise error code
 */
int32_t __Blocker_notify(CBlocker* blocker);

/**
 * notify all waiting blocker
 * @param blocker blocker object
 * @return 0 if successfully, otherwise error code
 */
int32_t __Blocker_notifyAll(CBlocker* blocker);

/**
 * release blocker attach data
 * @param blocker blocker object
 * @param externalLock flag indicates whether blocker's mutex
 * if specified by external parameter or by inner create
 */
void __Blocker_finalize(CBlocker* blocker);

#ifdef __cplusplus
}
#endif
