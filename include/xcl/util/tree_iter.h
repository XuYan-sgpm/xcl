//
// Created by 14049 on 2024/6/24.
//

#pragma once

#include <stdbool.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void* cur;
  void* impl;
} TreeIter;

TreeIter TreeIter_make(void* node, void* impl);
TreeIter TreeIter_next(TreeIter it);
TreeIter TreeIter_prev(TreeIter it);
bool TreeIter_equal(TreeIter it1, TreeIter it2);
bool TreeIter_valid(TreeIter it);
void* TreeIter_key(TreeIter it);
void* TreeIter_value(TreeIter it);

#ifdef __cplusplus
}
#endif
