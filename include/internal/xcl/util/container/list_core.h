//
// Created by 徐琰 on 2022/1/3.
//

#ifndef SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_LIST_CORE_H_
#define SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_LIST_CORE_H_

#include "list_def.h"
#include "xcl/util/container/objects_handler.h"

namespace xcl {
void
ListLink(ListNode *node1, ListNode *node2);
void
ListAdd(ListNode *pos, ListNode *node);
void
ListAdd(ListNode *pos, ListNode *first, ListNode *last);
void
ListRemove(ListNode *pos);
void
ListRemove(ListNode *first, ListNode *last);
}// namespace xcl

#endif// SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_LIST_CORE_H_
