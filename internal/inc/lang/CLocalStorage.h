//
// Created by xuyan on 2022/6/21.
//

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * local storage is not responsible for object
 * memory management
 * each block store 8 bytes
 * for element size less than 8
 * or store buffer or object pointer
 * each block store one thread local data
 */
typedef struct {
  char data[8];
} Block;

typedef struct {
  Block *blocks;
  int32_t cap;
} CLocalStorage;

/**
 * set ptr to local storage, treat ptr address as data
 * instead of data pointer to ptr
 * @param localStorage local storage object
 * @param idx data push position
 * @param ptr pushed pointer
 * @return true if set successfully, false otherwise
 */
bool LocalStorage_setPtr(CLocalStorage *localStorage, int idx, intptr_t ptr);

/**
 * copy buffer to local storage if buffer size less than 8
 * @param localStorage local storage object
 * @param idx buffer push position
 * @param src buffer address
 * @param len buffer size
 * @return true if push buffer successfully, false otherwise
 */
bool LocalStorage_setTiny(CLocalStorage *localStorage,
                          int idx,
                          const void *src,
                          int len);

/**
 * get data address at position idx of local storage
 * @param localStorage local storage object
 * @param idx data position
 * @return local data address if successfully, otherwise false
 */
void *LocalStorage_get(CLocalStorage *localStorage, int idx);

/**
 * release local storage memory, not include
 * memory pointer to ptr which stored in local storage
 * @param localStorage local storage object
 */
void LocalStorage_free(CLocalStorage *localStorage);

/**
 * get thread local storage
 * @return local storage bind to current thread
 * if local storage of current thread initialization
 * failed, return NULL
 */
CLocalStorage *__ThreadLocal_getLocalStorage();

/**
 * bind local storage with current thread
 * @param localStorage local thread object
 * @return if bind local storage with current thread
 * successfully, return true; otherwise return false
 */
bool __ThreadLocal_setLocalStorage(CLocalStorage *localStorage);

#ifdef __cplusplus
}
#endif
