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
#include <xcl/lang/define.h>

typedef struct _ListNode ListNode;
struct _ListNode {
  ListNode* prev;
  ListNode* next;
};

/**
 * insert list opaque before index
 * @param pos insert position
 * @param node list opaque
 */
void List_add(ListNode* pos, ListNode* node);

/**
 * entry node1 with node2
 * @param node1 list node1
 * @param node2 list node2
 */
void List_link(ListNode* node1, ListNode* node2);

/**
 * remove list opaque
 * @param node list opaque
 * return next list opaque
 */
void List_remove(ListNode* node);

/**
 * sort list
 * @param node first opaque of list
 * @param compare list opaque compare callback
 * @return first opaque of list
 */
ListNode* List_sort(ListNode* node, int (*compare)(const ListNode*, const ListNode*));

typedef struct {
  ListNode header;
  unsigned size;
} List;

/**
 * initialize list object
 * @param list list object
 */
void List_init(List* list);

/**
 * get first opaque of list
 * @param list list object
 * @return first list opaque
 */
ListNode* List_first(List* list);

/**
 * get last opaque of list
 * @param list list object
 * @return last list opaque
 */
ListNode* List_last(List* list);

/**
 * add list opaque at first position of list
 * @param list list object
 * @param node new list opaque
 */
void List_addFront(List* list, ListNode* node);

/**
 * add list opaque at last position of list
 * @param list list object
 * @param node new list opaque
 */
void List_addBack(List* list, ListNode* node);

/**
 * pop the front opaque of list
 * @param list list object
 * @return first list opaque
 */
ListNode* List_popFront(List* list);

/**
 * pop the last opaque of list
 * @param list list object
 * @return last list opaque
 */
ListNode* List_popBack(List* list);

/**
 * add new opaque before specified position of list
 * @param list list object
 * @param pos position opaque
 * @param node new list opaque
 */
void List_insert(List* list, ListNode* pos, ListNode* node);

/**
 * erase opaque at specified position of list
 * @param list list object
 * @param pos position opaque
 */
void List_erase(List* list, ListNode* pos);

/**
 * splice source into list
 * @param list list object
 * @param pos position opaque
 * @param source source list
 */
void List_spliceAll(List* list, ListNode* pos, List* source);

/**
 * splice one list node from source to list
 * @param list dest list
 * @param pos splice position
 * @param source source list
 * @param node splice node
 */
void List_splice(List* list, ListNode* pos, List* source, ListNode* node);

/**
 * get if list is empty
 * @param list list object
 * @return true if list is empty, otherwise false
 */
bool List_empty(List* list);

/**
 * sort list
 * @param list list object
 */
void List_sort2(List* list, int (*compare)(const ListNode*, const ListNode*));

/**
 * swap two list object
 * @param list1
 * @param list2
 */
void List_swap(List* list1, List* list2);

void List_traverse(List* list, void (*node_handler)(ListNode*));

#ifdef __cplusplus
}
#endif
