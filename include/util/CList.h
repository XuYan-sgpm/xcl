#pragma once

/*
 * author: xuyan
 * time: 2022/05/09
 * a simple generic bidirectional list implementation
 */
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CListNode {
  struct _CListNode *next;
  struct _CListNode *prev;
  char data[0];
} CListNode;

typedef struct _CList CList;

typedef struct {
  CListNode *terminator;
  CListNode *cur;
} CListIter;

/*
 * create a list object
 */
CList *makeList();

/*
 * free list if list is empty
 * otherwise return false
 */
bool freeList(CList *list);

/*
 * return begin iter of list
 */
CListIter List_begin(CList *list);

/*
 * return end iter of list
 */
CListIter List_end(CList *list);

/*
 * return previous iter of it
 */
CListIter List_prev(CListIter it);

/*
 * return next iter of it
 */
CListIter List_next(CListIter it);

/*
 * compare list iter
 */
bool List_iterEquals(CListIter it1, CListIter it2);

/*
 * add value before pos
 */
bool List_add(CList *list, CListIter pos, CListNode *node);

/*
 * push value back into list
 */
void List_push(CList *list, CListNode *node);

/*
 * push value into the first pos of list
 */
void List_pushFront(CList *list, CListNode *node);

/*
 * pop the first node of list
 */
CListNode *List_pop(CList *list);

/*
 * pop last node of list
 */
CListNode *List_popBack(CList *list);

/*
 * get the first iter of list
 */
CListIter List_peekFirst(CList *list);

/*
 * get the last iter of list
 */
CListIter List_peekLast(CList *list);

/*
 * remove node which contains data equals to value
 */
bool List_remove(CList *list,
                 const void *value,
                 int (*cmp)(const void *, const void *));

/*
 * use cmpHook to find node contains data equals to value
 * if found, return node; otherwise return NULL
 */
CListIter List_query(CList *list,
                     const void *value,
                     int (*cmp)(const void *, const void *));

/*
 * similar to List_query, but start find progress at pos
 */
CListIter List_query2(CList *list,
                      CListIter pos,
                      const void *value,
                      int (*cmp)(const void *, const void *));

/*
 * similar to List_remove, but remove all nodes which contains data
 * equals to value
 */
int32_t List_removeAll(CList *list,
                       const void *value,
                       int (*cmp)(const void *, const void *));

/*
 * return size of list, may be O(n)
 */
uint32_t List_size(CList *list);

/*
 * return if list is empty
 */
bool List_empty(CList *list);

/*
 * cut list node range [first, last) and put
 * before pos
 */
bool List_spliceRange(
    CList *list, CListIter pos, CList *other, CListIter first, CListIter last);

/*
 * unlink node, and put node before pos
 */
bool List_splice(CList *list, CListIter pos, CList *other, CListIter it);

/*
 * cut all nodes in list and put these nodes before pos
 */
bool List_spliceAll(CList *list, CListIter pos, CList *other);

/*
 * sort elements contains in list
 */
void List_sort(CList *list, int (*cmp)(const void *, const void *));

#ifdef __cplusplus
}
#endif
