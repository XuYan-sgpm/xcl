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
CListIter listBegin(CList *list);

/*
 * return end iter of list
 */
CListIter listEnd(CList *list);

/*
 * return previous iter of it
 */
CListIter listPrev(CListIter it);

/*
 * return next iter of it
 */
CListIter listNext(CListIter it);

/*
 * compare list iter
 */
bool listIterEquals(CListIter it1, CListIter it2);

/*
 * add value before pos
 */
bool listAdd(CList *list, CListIter pos, CListNode *node);

/*
 * push value back into list
 */
void listPush(CList *list, CListNode *node);

/*
 * push value into the first pos of list
 */
void listPushFront(CList *list, CListNode *node);

/*
 * pop the first node of list
 */
CListNode *listPop(CList *list);

/*
 * pop last node of list
 */
CListNode *listPopBack(CList *list);

/*
 * get the first iter of list
 */
CListIter listPeekFirst(CList *list);

/*
 * get the last iter of list
 */
CListIter listPeekLast(CList *list);

/*
 * remove node which contains data equals to value
 */
bool listRemove(CList *list,
                const void *value,
                int (*cmp)(const void *, const void *));

/*
 * use cmpHook to find node contains data equals to value
 * if found, return node; otherwise return NULL
 */
CListIter listQuery(CList *list,
                    const void *value,
                    int (*cmp)(const void *, const void *));

/*
 * similar to listQuery, but start find progress at pos
 */
CListIter listQuery2(CList *list,
                     CListIter pos,
                     const void *value,
                     int (*cmp)(const void *, const void *));

/*
 * similar to listRemove, but remove all nodes which contains data
 * equals to value
 */
int32_t listRemoveAll(CList *list,
                      const void *value,
                      int (*cmp)(const void *, const void *));

/*
 * return size of list, may be O(n)
 */
uint32_t listSize(CList *list);

/*
 * return if list is empty
 */
bool listEmpty(CList *list);

/*
 * cut list node range [first, last) and put
 * before pos
 */
bool listSpliceRange(
    CList *list, CListIter pos, CList *other, CListIter first, CListIter last);

/*
 * unlink node, and put node before pos
 */
bool listSplice(CList *list, CListIter pos, CList *other, CListIter it);

/*
 * cut all nodes in list and put these nodes before pos
 */
bool listSpliceAll(CList *list, CListIter pos, CList *other);

/*
 * sort elements contains in list
 */
void listSort(CList *list, int (*cmp)(const void *, const void *));

#ifdef __cplusplus
}
#endif
