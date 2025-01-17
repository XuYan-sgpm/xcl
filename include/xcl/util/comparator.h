//
// Created by 14049 on 2024/6/17.
//

#pragma once
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Comparator Comparator;
struct Comparator {
  struct {
    int (*compare)(Comparator*, const void*, const void*);
  }* __vptr;
};

inline always_inline int Comparator_cmp(Comparator* comparator, const void* obj1,
                                        const void* obj2) {
  return comparator->__vptr->compare(comparator, obj1, obj2);
}

#ifdef __cplusplus
}
#endif
