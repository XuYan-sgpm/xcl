//
// Created by xuyan on 2022/6/21.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/xcl_def.h"
#include <stdbool.h>
#include <stdint.h>

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
} StorageBlock;

typedef struct {
    StorageBlock* blocks;
    int32_t cap;
} LocalStorage;

/**
 * create a thread local storage
 * @return local storage
 */
LocalStorage*
LocalStorage_new();

/**
 * set ptr to local storage, treat ptr address as data
 * instead of data pointer to ptr
 * @param localStorage local storage object
 * @param idx data push position
 * @param ptr pushed pointer
 * @return true if set successfully, false otherwise
 */
bool
LocalStorage_setPtr(LocalStorage* localStorage, int idx, intptr_t ptr);

/**
 * copy buffer to local storage if buffer size less than 8
 * @param localStorage local storage object
 * @param idx buffer push position
 * @param src buffer address
 * @param len buffer size
 * @return true if push buffer successfully, false otherwise
 */
bool
LocalStorage_setTiny(LocalStorage* localStorage,
                     int idx,
                     const void* src,
                     int len);

/**
 * get data address at position idx of local storage
 * @param localStorage local storage object
 * @param idx data position
 * @return local data address if successfully, otherwise false
 */
void*
LocalStorage_get(LocalStorage* localStorage, int idx);

/**
 * release local storage memory, not include
 * memory pointer to ptr which stored in local storage
 * @param localStorage local storage object
 */
void
LocalStorage_delete(LocalStorage* localStorage);

#ifdef __cplusplus
}
#endif
