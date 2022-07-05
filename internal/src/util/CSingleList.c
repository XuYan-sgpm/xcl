//
// Created by xuyan on 2022/6/23.
//

#include <string.h>
#include "util/CSingleList.h"

CSingleList SingleList_new() {
  CSingleList list;
  memset(&list.header, 0, sizeof(CSingleNode));
  return list;
}

bool SingleList_empty(const CSingleList *list) {
  return list->header.next == NULL;
}

int32_t SingleList_size(const CSingleList *list) {
  int32_t count = 0;
  CSingleNode *cur = list->header.next;
  while (cur) {
    cur = cur->next;
    ++count;
  }
  return count;
}
CSingleListIter SingleList_begin(CSingleList *list) {
  CSingleListIter iter = {.tag = &list->header, .cur = list->header.next};
  return iter;
}
CSingleListIter SingleList_end(CSingleList *list) {
  CSingleListIter iter = {.tag = &list->header, .cur = &list->header};
  return iter;
}
void SingleList_pushFront(CSingleList *list, CSingleNode *node) {
  node->next = list->header.next;
  list->header.next = node;
}
void SingleList_pushBack(CSingleList *list, CSingleNode *node) {
  CSingleNode *cur = &list->header;
  while (cur->next) {
    cur = cur->next;
  }
  cur->next = node;
  node->next = NULL;
}
CSingleNode *SingList_popFront(CSingleList *list) {
  CSingleNode *node = list->header.next;
  if (node) {
    list->header.next = node->next;
  }
  return node;
}
CSingleNode *SingleList_popBack(CSingleList *list) {
  CSingleNode *prev = &list->header;
  CSingleNode *node = prev->next;
  if (!node) {
    return NULL;
  }
  while (node->next) {
    prev = node;
    node = node->next;
  }
  prev->next = NULL;
  return node;
}
CSingleListIter SingleList_next(CSingleList *list, CSingleListIter iter) {
  if (!iter.cur || iter.cur == iter.tag) {
    return iter;
  }
  CSingleListIter next = iter;
  if (next.cur->next) {
    next.cur = next.cur->next;
  } else {
    next.cur = next.tag;
  }
  return next;
}

void SingleList_sort(CSingleList *list,
                     int (*cmp)(const void *, const void *)) {
  //
}
