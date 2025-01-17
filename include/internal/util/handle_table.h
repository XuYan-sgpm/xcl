//
// Created by 14049 on 2024/6/13.
//

#pragma once

#include <internal/util/handle_pool.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HandleTable HandleTable;
HandleTable* HandleTable_new(HandlePool* pool);
void HandleTable_free(HandleTable* handle_table);
void* HandleTable_map(HandleTable* handle_table, void* obj, void (*destructor)(void*),
                      const char* name);
void HandleTable_close(HandleTable* handle_table, void* h);
void* HandleTable_get(HandleTable* handle_table, void* h);
void* HandleTable_clone(HandleTable* handle_table, void* h);
void* HandleTable_cloneByName(HandleTable* handle_table, const char* name);
bool HandleTable_contains(HandleTable* handle_table, const char* name);
void HandleTable_closeByName(HandleTable* handle_table, const char* name);
void HandleTable_clear(HandleTable* handle_table);

#ifdef __cplusplus
}
#endif
