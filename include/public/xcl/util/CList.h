#pragma once

/*
 * author: xuyan
 * time: 2022/05/09
 * a simple generic bidirectional list implementation
 */
#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/XclDef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CListNode {
    struct _CListNode* next;
    struct _CListNode* prev;
    char data[0];
} CListNode;

typedef struct _CList CList;

typedef struct {
    CListNode* terminator;
    CListNode* cur;
} CListIter;

/*
 * create a list object
 */
XCL_PUBLIC(CList*)
List_new();

/*
 * free list if list is empty
 * otherwise return false
 */
XCL_PUBLIC(bool)
List_delete(CList* list);

/*
 * return begin iter of list
 */
XCL_PUBLIC(CListIter)
List_begin(CList* list);

/*
 * return end iter of list
 */
XCL_PUBLIC(CListIter)
List_end(CList* list);

/*
 * return previous iter of it
 */
XCL_PUBLIC(CListIter)
List_prev(CListIter it);

/*
 * return next iter of it
 */
XCL_PUBLIC(CListIter)
List_next(CListIter it);

/*
 * compare list iter
 */
XCL_PUBLIC(bool)
List_iterEquals(CListIter it1, CListIter it2);

/*
 * add value before pos
 */
XCL_PUBLIC(bool)
List_add(CList* list, CListIter pos, CListNode* node);

/*
 * push value back into list
 */
XCL_PUBLIC(void)
List_push(CList* list, CListNode* node);

/*
 * push value into the first pos of list
 */
XCL_PUBLIC(void)
List_pushFront(CList* list, CListNode* node);

/*
 * pop the first node of list
 */
XCL_PUBLIC(CListNode*)
List_pop(CList* list);

/*
 * pop last node of list
 */
XCL_PUBLIC(CListNode*)
List_popBack(CList* list);

/*
 * get the first iter of list
 */
XCL_PUBLIC(CListIter)
List_peekFirst(CList* list);

/*
 * get the last iter of list
 */
XCL_PUBLIC(CListIter)
List_peekLast(CList* list);

/**
 * remote node at list iterator, but no delete node data
 * @param list list object
 * @param iter list iterator
 * @return true if remove successfully, otherwise false
 */
XCL_PUBLIC(CListIter)
List_remove(CList* list, CListIter iter);

/*
 * use cmpHook to find node contains data equals to value
 * if found, return node; otherwise return NULL
 */
XCL_PUBLIC(CListIter)
List_query(CList* list, const void* value,
           int (*cmp)(const void*, const void*));

/*
 * similar to List_query, but start find progress at pos
 */
XCL_PUBLIC(CListIter)
List_query2(CList* list, CListIter pos, const void* value,
            int (*cmp)(const void*, const void*));

/*
 * return size of list, may be O(n)
 */
XCL_PUBLIC(uint32_t)
List_size(CList* list);

/*
 * return if list is empty
 */
XCL_PUBLIC(bool)
List_empty(CList* list);

/*
 * cut list node range [first, last) and put
 * before pos
 */
XCL_PUBLIC(bool)
List_spliceRange(CList* list, CListIter pos, CList* other, CListIter first,
                 CListIter last);

/*
 * unlink node, and put node before pos
 */
XCL_PUBLIC(bool)
List_splice(CList* list, CListIter pos, CList* other, CListIter it);

/*
 * cut all nodes in list and put these nodes before pos
 */
XCL_PUBLIC(bool)
List_spliceAll(CList* list, CListIter pos, CList* other);

/*
 * sort elements contains in list
 */
XCL_PUBLIC(void)
List_sort(CList* list, int (*cmp)(const void*, const void*));

#ifdef __cplusplus
}
#endif
