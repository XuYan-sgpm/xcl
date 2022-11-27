#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <xcl/pool/pool.h>
#include <xcl/lang/xcl_def.h>

typedef struct TrieTree TrieTree;

typedef struct TrieIter {
    uintptr_t data[2];
} TrieIter;

XCL_EXPORT TrieTree* XCL_API
TrieTree_new(int32_t size, Pool* pool);

XCL_EXPORT void XCL_API
TrieTree_delete(TrieTree* tree);

XCL_EXPORT void* XCL_API
TrieTree_get(TrieTree* tree, const char* word);

XCL_EXPORT void* XCL_API
TrieTree_check(TrieTree* tree, const char* word);

XCL_EXPORT int32_t XCL_API
TrieTree_size(TrieTree* tree);

XCL_EXPORT bool XCL_API
TrieTree_contains(TrieTree* tree, const char* word);

XCL_EXPORT TrieIter XCL_API
TrieTree_find(TrieTree* tree, const char* word);

XCL_EXPORT void* XCL_API
TrieTree_value(TrieTree* tree, TrieIter iter);

XCL_EXPORT bool XCL_API
TrieTree_remove(TrieTree* tree, const char* word);

XCL_EXPORT void XCL_API
TrieTree_removeAt(TrieTree* tree, TrieIter iter);

XCL_EXPORT bool XCL_API
TrieIter_valid(TrieIter iter);

#ifdef __cplusplus
}
#endif
