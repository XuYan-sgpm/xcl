//
// Created by xuyan on 2022/6/23.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "lang/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CSingleNode {
  struct _CSingleNode *next;
  char data[0];
} CSingleNode;

typedef struct {
  CSingleNode header;
} CSingleList;

typedef struct {
  /*
   * pointer to an object which can prove
   * relationship between iter and list
   */
  void *const tag;
  CSingleNode *cur;
} CSingleListIter;

/**
 * create a single list object
 * @return single list object
 */
CSingleList XCL_API SingleList_new();

/**
 * check if single list is empty
 * @param list single list object
 * @return true if list is empty, otherwise false
 */
bool XCL_API SingleList_empty(const CSingleList *list);

/**
 * get single list element count
 * @param list single list object
 * @return element count of list
 */
int32_t XCL_API SingleList_size(const CSingleList *list);

/**
 * begin iter of single list
 * @param list single list object
 * @return iter of begin position of single list
 */
CSingleListIter XCL_API SingleList_begin(CSingleList *list);

/**
 * end iter of single list
 * @param list single list object
 * @return iter of last position of single list,
 * without list data
 */
CSingleListIter XCL_API SingleList_end(CSingleList *list);

/**
 * push node before begin position of single list
 * @param list single list object
 * @param node node contains data
 */
void XCL_API SingleList_pushFront(CSingleList *list, CSingleNode *node);

/**
 * push node at last position of single list
 * @param list single list object
 * @param node node contains data
 */
void XCL_API SingleList_pushBack(CSingleList *list, CSingleNode *node);

/**
 * pop first node of single list
 * @param list single list object
 * @return first node of single list
 */
CSingleNode *XCL_API SingList_popFront(CSingleList *list);

/**
 * pop last node contains data of single list
 * @param list single list object
 * @return last node contains value
 */
CSingleNode *XCL_API SingleList_popBack(CSingleList *list);

/**
 * next iter of passing iter
 * @param list single list object
 * @param iter current list iter
 * @return next iter
 */
CSingleListIter XCL_API SingleList_next(CSingleList *list,
                                        CSingleListIter iter);

/**
 * sort single list
 * @param list single list object
 * @param cmp compare function pointer
 */
void XCL_API SingleList_sort(CSingleList *list,
                             int (*cmp)(const void *, const void *));

#ifdef __cplusplus
}
#endif
