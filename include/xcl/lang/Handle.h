#pragma once

#include <stdint.h>
#include <xcl/lang/XclDef.h>
#include <xcl/pool/Pool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CHandleTable CHandleTable;

/**
 * @brief new a handle table with level
 * @param level table level, each level can store 512 pointer
 * @param pool pool object
 * @return handle table
 */
XCL_EXPORT CHandleTable* XCL_API
HandleTable_new(int32_t level, CPool* pool);

/**
 * @brief capacity of handle table
 * @param table handle table
 * @return max handle count which table can hold
 */
XCL_EXPORT uint64_t XCL_API
HandleTable_cap(const CHandleTable* table);

/**
 * @brief push object into handle table
 * @param table handle table
 * @param obj object
 * @return object handle
 */
XCL_EXPORT void* XCL_API
HandleTable_pushObj(CHandleTable* table, void* obj);

/**
 * @brief discard object handle
 * @param handle object handle
 */
XCL_EXPORT void XCL_API
HandleTable_discard(void* handle);

/**
 * @brief delete handle table
 * @param table handle table object
 */
XCL_EXPORT void XCL_API
HandleTable_delete(CHandleTable* table);

#ifdef __cplusplus
}
#endif
