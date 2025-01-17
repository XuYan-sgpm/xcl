//
// Created by 徐琰 on 2023/10/17.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

int Local_new();
bool Local_set(int id, void* attach, void (*destructor)(void*));
void* Local_get(int id);
void Local_free(int id);

#ifdef __cplusplus
};
#endif
