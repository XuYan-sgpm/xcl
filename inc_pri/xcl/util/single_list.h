//
// Created by xuyan on 2022/6/23.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/xcl_def.h"
#include "xcl/pool/pool.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct SingleNode {
    struct SingleNode* next;
    char data[0];
} SingleNode;

typedef struct SingleList SingleList;

typedef struct {
    /*
     * pointer to an object which can prove
     * relationship between iter and list
     */
    void* const tag;
    SingleNode* cur;
} SingleListIter;

/**
 * new a single list object
 * @return single list object
 */
XCL_EXPORT SingleList* XCL_API
SingleList_new();

/**
 * @brief create single list by specified pool
 * @param pool struct Pool object
 * @return single list object if create successfully, otherwise NULL
 * @author xuyan
 * @date 2022-08-05
 */
XCL_EXPORT SingleList* XCL_API
SingleList_newByPool(struct Pool* pool);

/**
 * delete list object
 * @param list single list object
 * @return true if list is empty and delete successfully,
 * otherwise false
 */
XCL_EXPORT bool XCL_API
SingleList_delete(SingleList* list);

/**
 * check if single list is empty
 * @param list single list object
 * @return true if list is empty, otherwise false
 */
XCL_EXPORT bool XCL_API
SingleList_empty(const SingleList* list);

/**
 * get single list element count
 * @param list single list object
 * @return element count of list
 */
XCL_EXPORT int32_t XCL_API
SingleList_size(const SingleList* list);

/**
 * begin iter of single list
 * @param list single list object
 * @return iter of begin position of single list
 */
XCL_EXPORT SingleListIter XCL_API
SingleList_begin(SingleList* list);

/**
 * size iter of single list
 * @param list single list object
 * @return iter of last position of single list,
 * without list data
 */
XCL_EXPORT SingleListIter XCL_API
SingleList_end(SingleList* list);

/**
 * push node before begin position of single list
 * @param list single list object
 * @param node node contains data
 */
XCL_EXPORT void XCL_API
SingleList_pushFront(SingleList* list, SingleNode* node);

/**
 * push node at last position of single list
 * @param list single list object
 * @param node node contains data
 */
XCL_EXPORT void XCL_API
SingleList_pushBack(SingleList* list, SingleNode* node);

/**
 * pop first node of single list
 * @param list single list object
 * @return first node of single list
 */
XCL_EXPORT SingleNode* XCL_API
SingleList_popFront(SingleList* list);

/**
 * pop last node contains data of single list
 * @param list single list object
 * @return last node contains value
 */
XCL_EXPORT SingleNode* XCL_API
SingleList_popBack(SingleList* list);

/**
 * next iter of passing iter
 * @param list single list object
 * @param iter current list iter
 * @return next iter
 */
XCL_EXPORT SingleListIter XCL_API
SingleList_next(SingleList* list, SingleListIter iter);

/**
 * sort single list
 * @param list single list object
 * @param cmp compare function pointer
 */
XCL_EXPORT void XCL_API
SingleList_sort(SingleList* list, int (*cmp)(const void*, const void*));

#ifdef __cplusplus
}
#endif
