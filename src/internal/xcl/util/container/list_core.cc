//
// Created by 徐琰 on 2022/1/3.
//

#include <xcl/util/container/list_core.h>

namespace xcl {
void
ListLink(ListNode *node1, ListNode *node2) {
  if (node1) { node1->next = node2; }
  if (node2) { node2->prev = node1; }
}
void
ListAdd(ListNode *pos, ListNode *node) {
  if (pos && node) {
	ListLink(pos->prev, node);
	ListLink(node, pos);
  }
}
void
ListAdd(ListNode *pos, ListNode *first, ListNode *last) {
  if (pos && first && last && first != last) {
	ListLink(pos->prev, first);
	ListLink(last->prev, pos);
  }
}
void
ListRemove(ListNode *pos) {
  if (pos) { ListLink(pos->prev, pos->next); }
}
void
ListRemove(ListNode *first, ListNode *last) {
  if (first && last && first != last) { ListLink(first->prev, last); }
}
}// namespace xcl
