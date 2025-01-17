//
// Created by xuyan on 2022/7/4.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_EXIT_HANDLER 256

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <xcl/io/path.h>
#include <xcl/lang/define.h>

/**
 * get current ticks
 * @return current cpu ticks
 */
int64_t currentMillis();

/**
 * get current ticks but in nano period
 * @return ticks for nano period
 */
int64_t nanos();

/**
 * sleep current thread during timeout milliseconds
 * @param timeout milliseconds timeout
 */
void msleep(int32_t timeout);

unsigned* _usr_err();
#define usr_errno (*_usr_err())

/**
 * @brief open module
 * @param path module path
 * @return module handle
 */
void* Dl_open(const char* path);

/**
 * @brief get module symbol address
 * @param handle module handle
 * @param symbol symbol name
 * @return symbol address
 */
void* Dl_sym(void* handle, const char* symbol);

/**
 * @brief close module
 * @param handle module handle
 */
void Dl_close(void* handle);

bool postExit(void (*f)());

struct _timeval {
  long tv_sec;
  long tv_usec;
};
int _gettimeofday(struct _timeval* tv, void* zone);

#ifdef __cplusplus
}
#endif
