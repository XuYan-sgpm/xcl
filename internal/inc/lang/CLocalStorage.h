//
// Created by xuyan on 2022/6/21.
//

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char data[8];
} Block;

typedef struct {
  Block *blocks;
  int32_t cap;
} CLocalStorage;

bool LS_SetPtr(CLocalStorage *localStorage, int idx, intptr_t ptr);
bool LS_SetTiny(CLocalStorage *localStorage, int idx, const void *src, int len);
void *LS_Get(CLocalStorage *localStorage, int idx);
void LS_Free(CLocalStorage *localStorage);

#ifdef __cplusplus
}
#endif
