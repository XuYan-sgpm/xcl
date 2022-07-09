//
// Created by xuyan on 2022/6/23.
//

#include <string.h>
#include "xcl/util/CSingleList.h"

CSingleList XCL_API
SingleList_new() {
  CSingleList list;
  memset(&list.header, 0, sizeof(CSingleNode));
  list.tail = &list.header;
  return list;
}

bool XCL_API
SingleList_empty(const CSingleList* list) {
  return list->header.next == NULL;
}

int32_t XCL_API
SingleList_size(const CSingleList* list) {
  int32_t count = 0;
  CSingleNode* cur = list->header.next;
  while (cur) {
    cur = cur->next;
    ++count;
  }
  return count;
}
CSingleListIter XCL_API
SingleList_begin(CSingleList* list) {
  CSingleListIter iter = {.tag = &list->header, .cur = list->header.next};
  return iter;
}
CSingleListIter XCL_API
SingleList_end(CSingleList* list) {
  CSingleListIter iter = {.tag = &list->header, .cur = &list->header};
  return iter;
}
void XCL_API
SingleList_pushFront(CSingleList* list, CSingleNode* node) {
  node->next = list->header.next;
  list->header.next = node;
  if (list->tail == &list->header)
    list->tail = node;
}
void XCL_API
SingleList_pushBack(CSingleList* list, CSingleNode* node) {
  list->tail->next = node;
  list->tail = node;
  node->next = NULL;
}
CSingleNode* XCL_API
SingList_popFront(CSingleList* list) {
  CSingleNode* node = list->header.next;
  if (node) {
    list->header.next = node->next;
    if (!list->header.next)
      list->tail = &list->header;
  }
  return node;
}
CSingleNode* XCL_API
SingleList_popBack(CSingleList* list) {
  CSingleNode* prev = &list->header;
  CSingleNode* node = prev->next;
  if (!node) {
    return NULL;
  }
  while (node->next) {
    prev = node;
    node = node->next;
  }
  prev->next = NULL;
  list->tail = prev;
  return node;
}
CSingleListIter XCL_API
SingleList_next(CSingleList* list, CSingleListIter iter) {
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

void XCL_API
SingleList_sort(CSingleList* list, int (*cmp)(const void*, const void*)) {
  //
}
