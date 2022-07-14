//
// Created by xuyan on 2022/6/23.
//

#pragma once

#include "xcl/lang/XclDef.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CSingleNode {
    struct _CSingleNode* next;
    char data[0];
} CSingleNode;

typedef struct _CSingleList_st CSingleList;

typedef struct {
    /*
     * pointer to an object which can prove
     * relationship between iter and list
     */
    void* const tag;
    CSingleNode* cur;
} CSingleListIter;

/**
 * new a single list object
 * @return single list object
 */
XCL_PUBLIC(CSingleList*)
SingleList_new();

/**
 * delete list object
 * @param list single list object
 */
XCL_PUBLIC(void)
SingleList_delete(CSingleList* list);

/**
 * check if single list is empty
 * @param list single list object
 * @return true if list is empty, otherwise false
 */
XCL_PUBLIC(bool)
SingleList_empty(const CSingleList* list);

/**
 * get single list element count
 * @param list single list object
 * @return element count of list
 */
XCL_PUBLIC(int32_t)
SingleList_size(const CSingleList* list);

/**
 * begin iter of single list
 * @param list single list object
 * @return iter of begin position of single list
 */
XCL_PUBLIC(CSingleListIter)
SingleList_begin(CSingleList* list);

/**
 * end iter of single list
 * @param list single list object
 * @return iter of last position of single list,
 * without list data
 */
XCL_PUBLIC(CSingleListIter)
SingleList_end(CSingleList* list);

/**
 * push node before begin position of single list
 * @param list single list object
 * @param node node contains data
 */
XCL_PUBLIC(void)
SingleList_pushFront(CSingleList* list, CSingleNode* node);

/**
 * push node at last position of single list
 * @param list single list object
 * @param node node contains data
 */
XCL_PUBLIC(void)
SingleList_pushBack(CSingleList* list, CSingleNode* node);

/**
 * pop first node of single list
 * @param list single list object
 * @return first node of single list
 */
XCL_PUBLIC(CSingleNode*)
SingleList_popFront(CSingleList* list);

/**
 * pop last node contains data of single list
 * @param list single list object
 * @return last node contains value
 */
XCL_PUBLIC(CSingleNode*)
SingleList_popBack(CSingleList* list);

/**
 * next iter of passing iter
 * @param list single list object
 * @param iter current list iter
 * @return next iter
 */
XCL_PUBLIC(CSingleListIter)
SingleList_next(CSingleList* list, CSingleListIter iter);

/**
 * sort single list
 * @param list single list object
 * @param cmp compare function pointer
 */
XCL_PUBLIC(void)
SingleList_sort(CSingleList* list, int (*cmp)(const void*, const void*));

#ifdef __cplusplus
}
#endif
