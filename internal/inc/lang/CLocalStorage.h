//
// Created by xuyan on 2022/6/21.
//

#pragma once

#ifdef STATIC_LIB

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char data[sizeof(void *) << 1];
} Block;

typedef struct {
  Block *blocks;
  int32_t cap;
} CLocalStorage;

bool setObj(CLocalStorage *localStorage,
            int idx,
            void *obj,
            void (*destructor)(void *));
bool setTiny(CLocalStorage *localStorage, int idx, const void *src, int len);
bool setBuffer(CLocalStorage *localStorage, int idx, const void *src, int len);
Block *get(CLocalStorage *localStorage, int idx);
void freeStorage(CLocalStorage *localStorage);

#ifdef __cplusplus
}
#endif

#endif
