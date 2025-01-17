//
// Created by 徐琰 on 2023/10/17.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void* attach;
  void (*destructor)(void*);
} LocalPair;

LocalPair LocalPair_make(void* a, void (*f)(void*));
void* LocalStorage_new();
void LocalStorage_free(void* obj);
bool LocalStorage_set(void* obj, uintptr_t id, LocalPair pair);
LocalPair* LocalStorage_get(void* obj, uintptr_t id);

#ifdef __cplusplus
};
#endif
