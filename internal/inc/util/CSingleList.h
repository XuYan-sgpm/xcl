//
// Created by xuyan on 2022/6/23.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * create a single list object
 * @return single list object
 */
CSingleList SingleList_new();

/**
 * check if single list is empty
 * @param list single list object
 * @return true if list is empty, otherwise false
 */
bool SingleList_empty(const CSingleList *list);

/**
 * get single list element count
 * @param list single list object
 * @return element count of list
 */
int32_t SingleList_size(const CSingleList *list);

/**
 * begin iter of single list
 * @param list single list object
 * @return iter of begin position of single list
 */
CSingleListIter SingleList_begin(CSingleList *list);

/**
 * end iter of single list
 * @param list single list object
 * @return iter of last position of single list,
 * without list data
 */
CSingleListIter SingleList_end(CSingleList *list);

/**
 * push node before begin position of single list
 * @param list single list object
 * @param node node contains data
 */
void SingleList_pushFront(CSingleList *list, CSingleNode *node);

/**
 * push node at last position of single list
 * @param list single list object
 * @param node node contains data
 */
void SingleList_pushBack(CSingleList *list, CSingleNode *node);

/**
 * pop first node of single list
 * @param list single list object
 * @return first node of single list
 */
CSingleNode *SingList_popFront(CSingleList *list);

/**
 * pop last node contains data of single list
 * @param list single list object
 * @return last node contains value
 */
CSingleNode *SingleList_popBack(CSingleList *list);

/**
 * next iter of passing iter
 * @param list single list object
 * @param iter current list iter
 * @return next iter
 */
CSingleListIter SingleList_next(CSingleList *list, CSingleListIter iter);

/**
 * prev iter of passing iter
 * @param list single list object
 * @param iter current list iter
 * @return prev iter
 */
CSingleListIter SingleList_prev(CSingleList *list, CSingleListIter iter);

/**
 * release single list resource if single list is empty
 * note that list node contains data we dont know how
 * to delete, so we can only release single list when
 * list is empty, if not, all list node data must be freed
 * by manual
 * @param list single list object
 * @return true if list is empty, otherwise false
 */
bool SingleList_delete(CSingleList *list);

/**
 * sort single list
 * @param list single list object
 * @param cmp compare function pointer
 */
void SingleList_sort(CSingleList *list, int (*cmp)(const void *, const void *));

#ifdef __cplusplus
}
#endif
