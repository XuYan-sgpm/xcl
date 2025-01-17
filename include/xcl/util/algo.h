//
// Created by 徐琰 on 2023/11/23.
//

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t roundUp(size_t size, size_t align);
size_t roundDown(size_t size, size_t align);
size_t log32(unsigned val);
size_t log64(uint64_t val);

/**
 * key_hash for bytes entry
 * @param data entry buffer
 * @param len entry bytes
 * @return key_hash result
 */
uint64_t hash(const void* data, size_t len);
uint64_t Snowflake_next();

struct Hasher {
  struct {
    uint64_t (*hash)(struct Hasher*, const void*);
  }* __vptr;
};

struct SimpleHasher {
  struct Hasher super;
  unsigned item_size;
};
struct SimpleHasher SimpleHasher_make(unsigned item_size);

const char* findLibrary(const char** names, unsigned name_size, const char** hints,
                        unsigned hint_size, char* path, unsigned path_len);

uintptr_t gcd(uintptr_t x, uintptr_t y);
uintptr_t lcm(uintptr_t x, uintptr_t y);

#ifdef __cplusplus
};
#endif
