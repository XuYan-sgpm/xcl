#include <stdio.h>
#include <string.h>
#include <xcl/util/list.h>

void List_add(ListNode* pos, ListNode* node) {
  if (pos) {
    List_link(pos->prev, node);
    List_link(node, pos);
  }
}

void List_link(ListNode* node1, ListNode* node2) {
  if (node1)
    node1->next = node2;
  if (node2)
    node2->prev = node1;
}

void List_remove(ListNode* node) {
  List_link(node->prev, node->next);
  node->prev = node->next = NULL;
}

static ListNode* __List_merge1(ListNode* node1, ListNode* node2,
                               int (*compare)(const ListNode*, const ListNode*)) {
  if (!node1 || !node2)
    return !node1 ? node2 : node1;
  ListNode dummy = {NULL, node1};
  ListNode* header1 = node1->prev;
  node1->prev = &dummy;
  ListNode* last1 = node1->prev;
  while (node1 && node2) {
    int ret = compare(node1, node2);
    if (ret <= 0) {
      last1 = node1;
      node1 = node1->next;
    } else {
      ListNode* next2 = node2->next;
      List_link(node1->prev, node2);
      List_link(node2, node1);
      node2 = next2;
    }
  }
  if (!node1 && node2)
    List_link(last1, node2);
  List_link(header1, dummy.next);
  return dummy.next;
}

ListNode* List_sort(ListNode* node, int (*compare)(const ListNode*, const ListNode*)) {
  if (!node)
    return NULL;
  ListNode* nodes[64] = {NULL};
  int last_idx = 0;
  ListNode *carry, *header = node->prev, *next;
  while (node) {
    next = node->next;
    node->next = node->prev = NULL;
    carry = node;
    int i = 0;
    for (; i < last_idx && nodes[i]; ++i) {
      carry = __List_merge1(carry, nodes[i], compare);
      nodes[i] = NULL;
    }
    nodes[i] = carry;
    if (i == last_idx)
      ++last_idx;
    node = next;
  }
  carry = NULL;
  for (int i = 0; i < last_idx; i++) {
    carry = __List_merge1(carry, nodes[i], compare);
  }
  List_link(header, carry);
  return carry;
}

void List_init(List* list) {
  list->header.prev = list->header.next = &list->header;
  list->size = 0;
}

ListNode* List_first(List* list) { return list->header.next; }

ListNode* List_last(List* list) { return &list->header; }

void List_addFront(List* list, ListNode* node) {
  List_link(node, list->header.next);
  List_link(&list->header, node);
  ++list->size;
}

void List_addBack(List* list, ListNode* node) {
  List_link(list->header.prev, node);
  List_link(node, &list->header);
  ++list->size;
}

ListNode* List_popFront(List* list) {
  ListNode* node = list->header.next;
  if (node != &list->header) {
    List_link(node->prev, node->next);
    node->prev = node->next = NULL;
    --list->size;
    return node;
  }
  return NULL;
}

ListNode* List_popBack(List* list) {
  ListNode* node = list->header.prev;
  if (node != &list->header) {
    List_link(node->prev, node->next);
    node->prev = node->next = NULL;
    --list->size;
    return node;
  }
  return NULL;
}

void List_insert(List* list, ListNode* pos, ListNode* node) {
  List_link(pos->prev, node);
  List_link(node, pos);
  ++list->size;
}

void List_erase(List* list, ListNode* pos) {
  List_link(pos->prev, pos->next);
  pos->prev = pos->next = NULL;
  --list->size;
}

void List_spliceAll(List* list, ListNode* pos, List* source) {
  if (source->size) {
    list->size += source->size;
    List_link(pos->prev, source->header.next);
    List_link(source->header.prev, pos);
    List_init(source);
  }
}

void List_splice(List* list, ListNode* pos, List* source, ListNode* node) {
  List_link(pos->prev, node);
  List_link(node, pos);
  ++list->size;
  --source->size;
}

bool List_empty(List* list) {
  return !list->size && list->header.next == &list->header && list->header.prev == &list->header;
}

void __List_merge2(List* list1, List* list2, int (*compare)(const ListNode*, const ListNode*)) {
  ListNode *node1 = list1->header.next, *node2 = list2->header.next, *last1 = &list1->header;
  while (node1 != &list1->header && node2 != &list2->header) {
    int ret = compare(node1, node2);
    if (ret <= 0) {
      last1 = node1;
      node1 = node1->next;
    } else {
      ListNode* next2 = node2->next;
      List_link(node1->prev, node2);
      List_link(node2, node1);
      node2 = next2;
    }
  }
  if (node1 == &list1->header && node2 != &list2->header) {
    List_link(last1, node2);
    List_link(list2->header.prev, &list1->header);
  }
  list1->size += list2->size;
  List_init(list2);
}

void List_sort2(List* list, int (*compare)(const ListNode*, const ListNode*)) {
  List buckets[65];
  for (int i = 0; i < 65; i++)
    List_init(buckets + i);
  List* carry = &buckets[64];
  ListNode *node = list->header.next, *next;
  int count = 0;
  while (node != &list->header) {
    next = node->next;
    List_addBack(carry, node);
    int i = 0;
    for (; i < count && !List_empty(&buckets[i]); i++) {
      __List_merge2(carry, buckets + i, compare);
      List_init(&buckets[i]);
    }
    List_spliceAll(&buckets[i], &buckets[i].header, carry);
    if (i == count)
      ++count;
    node = next;
  }
  List_init(list);
  List_init(carry);
  int idx = 0;
  for (; idx < count; ++idx) {
    __List_merge2(list, &buckets[idx], compare);
  }
}

void List_swap(List* list1, List* list2) {
  if (list1->size || list2->size) {
    List tmp;
    List_init(&tmp);
    List_spliceAll(&tmp, &tmp.header, list1);
    List_spliceAll(list1, &list1->header, list2);
    List_spliceAll(list2, &list2->header, &tmp);
  }
}

void List_traverse(List* list, void (*node_handler)(ListNode*)) {
  ListNode* cur = list->header.next;
  while (cur != &list->header) {
    ListNode* next = cur->next;
    node_handler(cur);
    cur = next;
  }
}
