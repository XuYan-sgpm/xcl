#pragma once

/*
 * author: xuyan
 * time: 2022/05/09
 * a simple generic bidirectional list implementation
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <lang/XclDef.h>

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
XCL_EXPORT CList* XCL_API
List_new();

/*
 * free list if list is empty
 * otherwise return false
 */
XCL_EXPORT bool XCL_API
List_delete(CList* list);

/*
 * return begin iter of list
 */
XCL_EXPORT CListIter XCL_API
List_begin(CList* list);

/*
 * return size iter of list
 */
XCL_EXPORT CListIter XCL_API
List_end(CList* list);

/*
 * return previous iter of it
 */
XCL_EXPORT CListIter XCL_API
List_prev(CListIter it);

/*
 * return next iter of it
 */
XCL_EXPORT CListIter XCL_API
List_next(CListIter it);

/*
 * compare list iter
 */
XCL_EXPORT bool XCL_API
List_iterEquals(CListIter it1, CListIter it2);

/*
 * add value before pos
 */
XCL_EXPORT bool XCL_API
List_add(CList* list, CListIter pos, CListNode* node);

/*
 * push value back into list
 */
XCL_EXPORT void XCL_API
List_push(CList* list, CListNode* node);

/*
 * push value into the first pos of list
 */
XCL_EXPORT void XCL_API
List_pushFront(CList* list, CListNode* node);

/*
 * pop the first node of list
 */
XCL_EXPORT CListNode* XCL_API
List_pop(CList* list);

/*
 * pop last node of list
 */
XCL_EXPORT CListNode* XCL_API
List_popBack(CList* list);

/*
 * get the first iter of list
 */
XCL_EXPORT CListIter XCL_API
List_peekFirst(CList* list);

/*
 * get the last iter of list
 */
XCL_EXPORT CListIter XCL_API
List_peekLast(CList* list);

/**
 * remote node at list iterator, but no delete node data
 * @param list list object
 * @param iter list iterator
 * @return true if remove successfully, otherwise false
 */
XCL_EXPORT CListIter XCL_API
List_remove(CList* list, CListIter iter);

/*
 * use cmpHook to find node contains data equals to value
 * if found, return node; otherwise return NULL
 */
XCL_EXPORT CListIter XCL_API
List_query(CList* list,
           const void* value,
           int (*cmp)(const void*, const void*));

/*
 * similar to List_query, but start find progress at pos
 */
XCL_EXPORT CListIter XCL_API
List_query2(CList* list,
            CListIter pos,
            const void* value,
            int (*cmp)(const void*, const void*));

/*
 * return size of list, may be O(n)
 */
XCL_EXPORT uint32_t XCL_API
List_size(CList* list);

/*
 * return if list is empty
 */
XCL_EXPORT bool XCL_API
List_empty(CList* list);

/*
 * cut list node range [first, last) and put
 * before pos
 */
XCL_EXPORT bool XCL_API
List_spliceRange(CList* list,
                 CListIter pos,
                 CList* other,
                 CListIter first,
                 CListIter last);

/*
 * unlink node, and put node before pos
 */
XCL_EXPORT bool XCL_API
List_splice(CList* list, CListIter pos, CList* other, CListIter it);

/*
 * cut all nodes in list and put these nodes before pos
 */
XCL_EXPORT bool XCL_API
List_spliceAll(CList* list, CListIter pos, CList* other);

/*
 * sort elements contains in list
 */
XCL_EXPORT void XCL_API
List_sort(CList* list, int (*cmp)(const void*, const void*));

#ifdef __cplusplus
}
#endif
