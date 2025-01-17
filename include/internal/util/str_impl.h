//
// Created by 14049 on 2024/6/3.
//

#pragma once

#include <stdbool.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  struct {
    uintptr_t size : PTR_BITS - 2;
    uintptr_t wide : 1;
    uintptr_t expanded : 1;
  };
  union {
    char tiny[16];
    struct {
      void* buf;
      uintptr_t cap;
    };
  };
} StrImpl;

uintptr_t StrImpl_cap(StrImpl* str_impl);
void StrImpl_destroy(StrImpl* str_impl);
StrImpl StrImpl_make(bool wide);
void* StrImpl_at(StrImpl* str_impl, uintptr_t pos);
bool StrImpl_insert(StrImpl* str_impl, uintptr_t pos, const void* s, uintptr_t n);
bool StrImpl_insertn(StrImpl* str_impl, uintptr_t pos, uintptr_t n, int val);
bool StrImpl_reserve(StrImpl* str_impl, uintptr_t cap);
bool StrImpl_push(StrImpl* str_impl, int val);
bool StrImpl_appendn(StrImpl* str_impl, uintptr_t n, int val);
bool StrImpl_append(StrImpl* str_impl, const void* s, uintptr_t n);
void* StrImpl_back(StrImpl* str_impl);
bool StrImpl_pop(StrImpl* str_impl, int* val);
bool StrImpl_filln(StrImpl* str_impl, uintptr_t n, int val);
bool StrImpl_fill(StrImpl* str_impl, const void* s, uintptr_t n);
bool StrImpl_erase(StrImpl* str_impl, uintptr_t pos, uintptr_t n);
void* StrImpl_terminate(StrImpl* str_impl);
intptr_t StrImpl_findc(StrImpl* str_impl, bool reverse, uintptr_t pos, int val);
intptr_t StrImpl_finds(StrImpl* str_impl, uintptr_t pos, const void* s);
int StrImpl_cmp(StrImpl* impl1, StrImpl* impl2);
int StrImpl_ncmp(StrImpl* seq1, uintptr_t pos1, StrImpl* seq2, uintptr_t pos2, uintptr_t n);

#ifdef __cplusplus
}
#endif
