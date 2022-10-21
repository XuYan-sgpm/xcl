#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <xcl/pool/pool.h>

typedef struct TrieTree TrieTree;

typedef struct TrieIter {
    uintptr_t data[2];
} TrieIter;

TrieTree*
TrieTree_new(int32_t size, Pool* pool);

void
TrieTree_delete(TrieTree* tree);

void*
TrieTree_get(TrieTree* tree, const char* word);

void*
TrieTree_check(TrieTree* tree, const char* word);

int32_t
TrieTree_size(TrieTree* tree);

bool
TrieTree_contains(TrieTree* tree, const char* word);

TrieIter
TrieTree_find(TrieTree* tree, const char* word);

void*
TrieTree_value(TrieTree* tree, TrieIter iter);

bool
TrieTree_remove(TrieTree* tree, const char* word);

void
TrieTree_removeAt(TrieTree* tree, TrieIter iter);

bool
TrieIter_valid(TrieIter iter);

#ifdef __cplusplus
}
#endif
