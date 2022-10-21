#pragma once

#include <stdint.h>
#include <xcl/pool/pool.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HandleTable HandleTable;

/**
 * @brief new a object table
 * @param pool pool object
 * @return object table
 */
struct HandleTable*
HandleTable_new(struct Pool* pool);

/**
 * @brief push object into object table
 * @param table object table
 * @param obj object
 * @param destructor function to destruct object when
 * close handle associated with object; passing null will
 * be ignored
 * @param name unique object name, can be null
 * name will be used when query object
 * @return object handle
 */
void*
HandleTable_add(struct HandleTable* table,
                void* obj,
                void (*destructor)(void*));

/**
 * @brief reference object associated with passing handle
 * and return a new handle
 * @param table object table
 * @param handle referenced object handle
 * @return new handle
 */
void*
HandleTable_ref(struct HandleTable* table, void* handle);

/**
 * @brief close object handle, if object reference count is 0,
 * release object by destructor
 * @param table object table
 * @param handle object handle
 */
void
HandleTable_close(struct HandleTable* table, void* handle);

/**
 * @brief get object by object handle
 * @param table object table
 * @param handle object handle
 * @return object associated with passing handle
 */
void*
HandleTable_get(struct HandleTable* table, void* handle);

/**
 * @brief delete object table, and close all handles and objects
 * @param table object table
 */
void
HandleTable_delete(struct HandleTable* table);

/**
 * @brief get if handle is exists in rbtree
 * @param table handle table
 * @param handle object handle
 * @return true if handle exists, otherwise false
 */
bool
HandleTable_valid(struct HandleTable* table, void* handle);

#ifdef __cplusplus
}
#endif
