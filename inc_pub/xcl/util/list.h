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
#include <xcl/lang/xcl_def.h>

typedef struct ListNode {
    struct ListNode* next;
    struct ListNode* prev;
    char data[0];
} ListNode;

typedef struct List List;

typedef struct {
    ListNode* terminator;
    ListNode* cur;
} ListIter;

/*
 * create a list object
 */
XCL_EXPORT List* XCL_API
List_new();

/*
 * free list if list is empty
 * otherwise return false
 */
XCL_EXPORT bool XCL_API
List_delete(List* list);

/*
 * return begin iter of list
 */
XCL_EXPORT ListIter XCL_API
List_begin(List* list);

/*
 * return size iter of list
 */
XCL_EXPORT ListIter XCL_API
List_end(List* list);

/*
 * return previous iter of it
 */
XCL_EXPORT ListIter XCL_API
List_prev(ListIter it);

/*
 * return next iter of it
 */
XCL_EXPORT ListIter XCL_API
List_next(ListIter it);

/*
 * compare list iter
 */
XCL_EXPORT bool XCL_API
List_iterEquals(ListIter it1, ListIter it2);

/*
 * add value before pos
 */
XCL_EXPORT bool XCL_API
List_add(List* list, ListIter pos, ListNode* node);

/*
 * push value back into list
 */
XCL_EXPORT void XCL_API
List_push(List* list, ListNode* node);

/*
 * push value into the first pos of list
 */
XCL_EXPORT void XCL_API
List_pushFront(List* list, ListNode* node);

/*
 * pop the first node of list
 */
XCL_EXPORT ListNode* XCL_API
List_pop(List* list);

/*
 * pop last node of list
 */
XCL_EXPORT ListNode* XCL_API
List_popBack(List* list);

/*
 * get the first iter of list
 */
XCL_EXPORT ListIter XCL_API
List_peekFirst(List* list);

/*
 * get the last iter of list
 */
XCL_EXPORT ListIter XCL_API
List_peekLast(List* list);

/**
 * remote node at list iterator, but no delete node data
 * @param list list object
 * @param iter list iterator
 * @return true if remove successfully, otherwise false
 */
XCL_EXPORT ListIter XCL_API
List_remove(List* list, ListIter iter);

/*
 * use cmpHook to find node contains data equals to value
 * if found, return node; otherwise return NULL
 */
XCL_EXPORT ListIter XCL_API
List_query(List* list, const void* value, int (*cmp)(const void*, const void*));

/*
 * similar to List_query, but start find progress at pos
 */
XCL_EXPORT ListIter XCL_API
List_query2(List* list,
            ListIter pos,
            const void* value,
            int (*cmp)(const void*, const void*));

/*
 * return size of list, may be O(n)
 */
XCL_EXPORT uint32_t XCL_API
List_size(List* list);

/*
 * return if list is empty
 */
XCL_EXPORT bool XCL_API
List_empty(List* list);

/*
 * cut list node range [first, last) and put
 * before pos
 */
XCL_EXPORT bool XCL_API
List_spliceRange(List* list,
                 ListIter pos,
                 List* other,
                 ListIter first,
                 ListIter last);

/*
 * unlink node, and put node before pos
 */
XCL_EXPORT bool XCL_API
List_splice(List* list, ListIter pos, List* other, ListIter it);

/*
 * cut all nodes in list and put these nodes before pos
 */
XCL_EXPORT bool XCL_API
List_spliceAll(List* list, ListIter pos, List* other);

/*
 * sort elements contains in list
 */
XCL_EXPORT void XCL_API
List_sort(List* list, int (*cmp)(const void*, const void*));

#ifdef __cplusplus
}
#endif
