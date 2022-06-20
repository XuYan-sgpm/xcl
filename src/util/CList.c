#include "util/CList.h"

#include <stdlib.h>

/*
 * link prev and next
 */
static inline void __listLink(CListNode *prev, CListNode *next) {
  if (prev) {
    prev->next = next;
  }
  if (next) {
    next->prev = prev;
  }
}

/*
 * unlink node from node's prev and its next
 * but not free
 */
static inline void __listUnlink(CListNode *node) {
  if (node) {
    if (node->prev) {
      node->prev->next = node->next;
    }
    if (node->next) {
      node->next->prev = node->prev;
    }
    node->next = node->prev = NULL;
  }
}

struct _CList {
  CListNode header;
};

static void __listInit(CList *list) {
  __listLink(&list->header, &list->header);
}

CList *makeList() {
  CList *list = (CList *)malloc(sizeof(CList));
  if (list) {
    __listInit(list);
  }
  return list;
}

bool freeList(CList *list) {
  if (!listEmpty(list)) {
    return false;
  }
  free(list);
  return true;
}

CListIter listBegin(CList *list) {
  CListIter it = {&list->header, list->header.next};
  return it;
}

CListIter listEnd(CList *list) {
  CListIter it = {&list->header, &list->header};
  return it;
}

CListIter listPrev(CListIter it) {
  CListIter prevIt = {it.terminator, it.cur->prev};
  return prevIt;
}

CListIter listNext(CListIter it) {
  CListIter nextIt = {it.terminator, it.cur->next};
  return nextIt;
}

void listPush(CList *list, CListNode *node) {
  __listLink(list->header.prev, node);
  __listLink(node, &list->header);
}

bool listIterEquals(CListIter it1, CListIter it2) { return it1.cur == it2.cur; }

bool listAdd(CList *list, CListIter pos, CListNode *node) {
  if (!node || pos.terminator != &list->header) {
    return false;
  }
  __listLink(pos.cur->prev, pos.cur);
  __listLink(node, pos.cur);
  return true;
}

void listPushFront(CList *list, CListNode *node) {
  if (!node) {
    return;
  }
  __listLink(node, list->header.next);
  __listLink(&list->header, node);
}

CListNode *listPop(CList *list) {
  CListNode *node = list->header.next;
  if (node == &list->header) {
    return NULL;
  }
  __listUnlink(node);
  return node;
}

CListNode *listPopBack(CList *list) {
  CListNode *node = list->header.prev;
  if (node != &list->header) {
    __listUnlink(node);
    return node;
  }
  return NULL;
}

CListIter listPeekFirst(CList *list) {
  CListIter it = {&list->header, &list->header};
  CListNode *node = list->header.next;
  if (node != it.terminator) {
    it.cur = node;
  }
  return it;
}

CListIter listPeekLast(CList *list) {
  CListIter it = {&list->header, &list->header};
  CListNode *node = list->header.prev;
  if (node != it.terminator) {
    it.cur = node;
  }
  return it;
}

bool listRemove(CList *list,
                const void *value,
                int (*cmp)(const void *, const void *)) {
  CListNode *cur = list->header.next;
  while (cur != &list->header) {
    int ret = cmp(cur->data, value);
    if (ret == 0) {
      __listUnlink(cur);
      return true;
    }
    cur = cur->next;
  }
  return false;
}

CListIter listQuery(CList *list,
                    const void *value,
                    int (*cmp)(const void *, const void *)) {
  CListNode *cur = list->header.next;
  CListIter it = {&list->header, &list->header};
  while (cur != &list->header) {
    if (cmp(cur->data, value) == 0) {
      it.cur = cur;
      break;
    }
    cur = cur->next;
  }
  return it;
}

CListIter listQuery2(CList *list,
                     CListIter pos,
                     const void *value,
                     int (*cmp)(const void *, const void *)) {
  CListIter it = listEnd(list);
  if (pos.terminator != &list->header) {
    return it;
  }
  CListNode *cur = pos.cur;
  while (cur != &list->header) {
    if (cmp(cur->data, value) == 0) {
      it.cur = cur;
      break;
    }
    cur = cur->next;
  }
  return it;
}

int32_t listRemoveAll(CList *list,
                      const void *value,
                      int (*cmp)(const void *, const void *)) {
  CListNode *cur = list->header.next;
  CListNode *next;
  int count = 0;
  while (cur != &list->header) {
    next = cur->next;
    if (cmp(cur->data, value) == 0) {
      // next = cur->next;
      __listUnlink(cur);
      // cur = next;
      ++count;
    }
    cur = next;
  }
  return count;
}

uint32_t listSize(CList *list) {
  int size = 0;
  CListNode *cur = list->header.next;
  while (cur != &list->header) {
    ++size;
  }
  return size;
}

bool listEmpty(CList *list) { return list->header.next == &list->header; }

bool listSpliceRange(
    CList *list, CListIter pos, CList *other, CListIter first, CListIter last) {
  if (pos.terminator != &list->header || first.terminator != &other->header ||
      last.terminator != &other->header) {
    return false;
  }
  if (last.cur == first.cur) {
    return true;
  }
  CListNode *lastRemoved = last.cur->prev;
  // if (lastRemoved == first.cur) {
  //   return;
  // }
  __listLink(first.cur->prev, last.cur);
  __listLink(pos.cur->prev, first.cur);
  __listLink(lastRemoved, pos.cur);
  return true;
}

bool listSplice(CList *list, CListIter pos, CList *other, CListIter it) {
  if (it.terminator != &other->header || pos.terminator != &list->header ||
      it.cur == it.terminator) {
    return false;
  }
  __listLink(it.cur->prev, it.cur->next);
  __listLink(pos.cur->prev, it.cur);
  __listLink(it.cur, pos.cur);
  return true;
}

bool listSpliceAll(CList *list, CListIter pos, CList *other) {
  if (pos.terminator != &list->header) {
    return false;
  }
  if (listEmpty(other)) {
    return true;
  }
  CListNode *first = other->header.next;
  CListNode *lastRemoved = other->header.prev;
  __listLink(first->prev, &other->header);
  __listLink(pos.cur->prev, first);
  __listLink(lastRemoved, pos.cur);
  return true;
}

/*
 * merge list2 into list1 with cmp
 */
static void __listMerge(CList *list1,
                        CList *list2,
                        int (*cmp)(const void *, const void *)) {
  CListIter it1 = listBegin(list1);
  CListIter it2 = listBegin(list2);
  while (it1.cur != it1.terminator && it2.cur != it2.terminator) {
    int ret = cmp(it1.cur->data, it2.cur->data);
    if (ret <= 0) {
      it1.cur = it1.cur->next;
    } else {
      CListIter it3 = listNext(it2);
      while (it3.cur != it3.terminator) {
        if (cmp(it1.cur->data, it3.cur->data) < 0) {
          break;
        }
        it3.cur = it3.cur->next;
      }
      listSpliceRange(list1, it1, list2, it2, it3);
      it2 = it3;
    }
  }
  if (it2.cur != it2.terminator) {
    listSpliceAll(list1, it1, list2);
  }
}

void listSort(CList *list, int (*cmp)(const void *, const void *)) {
  if (listEmpty(list) || list->header.next->next == &list->header) {
    return;
  }
  CList carry;
  CList tmp[64];
  for (int i = 0; i < 64; i++) {
    __listInit(&tmp[i]);
  }
  __listInit(&carry);
  int fill = 0;
  while (!listEmpty(list)) {
    listSplice(&carry, listEnd(&carry), list, listBegin(list));
    int i = 0;
    while (!listEmpty(&tmp[i])) {
      __listMerge(&tmp[i], &carry, cmp);
      listSpliceAll(&carry, listBegin(&carry), &tmp[i++]);
    }
    listSpliceAll(&tmp[i], listBegin(&tmp[i]), &carry);
    if (i > fill) {
      fill = i;
    }
  }
  for (int i = 1; i <= fill; i++) {
    if (!listEmpty(&tmp[i])) {
      __listMerge(&tmp[0], &tmp[i], cmp);
    }
  }
  listSpliceAll(list, listEnd(list), &tmp[0]);
}
