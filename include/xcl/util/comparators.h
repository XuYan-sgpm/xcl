#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/util/comparator.h>

typedef struct {
  Comparator super;
  unsigned item_size;
} SimpleComparator;

SimpleComparator SimpleComparator_make(unsigned item_size);
Comparator* i8comp();
Comparator* u8comp();
Comparator* i16comp();
Comparator* u16comp();
Comparator* i32comp();
Comparator* u32comp();
Comparator* f32comp();
Comparator* i64comp();
Comparator* u64comp();
Comparator* f64comp();
Comparator* scomp();
Comparator* pcomp();
Comparator* stringcomp();

#ifdef __cplusplus
}
#endif
