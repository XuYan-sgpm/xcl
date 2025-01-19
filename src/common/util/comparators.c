#include <string.h>
#include <xcl/util/comparators.h>
#include <xcl/util/string.h>

static int SimpleComparator_comp(Comparator* obj, const void* x, const void* y) {
  return memcmp(x, y, ((SimpleComparator*)obj)->item_size);
}
SimpleComparator SimpleComparator_make(unsigned item_size) {
  static void* simple_comp_vptr[] = {SimpleComparator_comp};
  SimpleComparator sc = {.item_size = item_size, .super.__vptr = (void*)simple_comp_vptr};
  return sc;
}

static int compare8i(Comparator* _, const void* o1, const void* o2) {
  return (int)(*(const char*)o1) - (int)(*(const char*)o2);
}
static int compare8u(Comparator* _, const void* o1, const void* o2) {
  return (int)(*(const uint8_t*)o1) - (int)(*(const uint8_t*)o2);
}
static int compare16i(Comparator* _, const void* o1, const void* o2) {
  return (int)(*(const short*)o1) - (int)(*(const short*)o2);
}
static int compare16u(Comparator* _, const void* o1, const void* o2) {
  return (int)(*(const uint16_t*)o1) - (int)(*(const uint16_t*)o2);
}
static int compare32i(Comparator* _, const void* o1, const void* o2) {
  return *(int32_t*)o1 == *(int32_t*)o2 ? 0 : *(int32_t*)o1 < *(int32_t*)o2 ? -1 : 1;
}
static int compare32u(Comparator* _, const void* o1, const void* o2) {
  return *(uint32_t*)o1 == *(uint32_t*)o2 ? 0 : *(uint32_t*)o1 < *(uint32_t*)o2 ? -1 : 1;
}
static int compare64i(Comparator* _, const void* o1, const void* o2) {
  return *(int64_t*)o1 == *(int64_t*)o2 ? 0 : *(int64_t*)o1 < *(int64_t*)o2 ? -1 : 1;
}
static int compare64u(Comparator* _, const void* o1, const void* o2) {
  return *(uint64_t*)o1 == *(uint64_t*)o2 ? 0 : *(uint64_t*)o1 < *(uint64_t*)o2 ? -1 : 1;
}
static int compare32f(Comparator* _, const void* o1, const void* o2) {
  return *(float*)o1 == *(float*)o2 ? 0 : *(float*)o1 < *(float*)o2 ? -1 : 1;
}
static int compare64f(Comparator* _, const void* o1, const void* o2) {
  return *(double*)o1 == *(double*)o2 ? 0 : *(double*)o1 < *(double*)o2 ? -1 : 1;
}
static int compares(Comparator* _, const void* o1, const void* o2) { return strcmp(o1, o2); }
static int comparep(Comparator* _, const void* o1, const void* o2) {
  return *(const uintptr_t*)o1 - *(const uintptr_t*)o2;
}
static int compareString(Comparator* _, const void* o1, const void* o2) {
  return strcmp(String_str((String*)o1), String_str((String*)o2));
}

static void* const compare_vptr_list[] = {
    compare8i,  compare8u,  compare16i, compare16u, compare32i, compare32u,   compare32f,
    compare64i, compare64u, compare64f, compares,   comparep,   compareString};
static Comparator comp_list[] = {
    {.__vptr = (void*)&compare_vptr_list[0]},  {.__vptr = (void*)&compare_vptr_list[1]},
    {.__vptr = (void*)&compare_vptr_list[2]},  {.__vptr = (void*)&compare_vptr_list[3]},
    {.__vptr = (void*)&compare_vptr_list[4]},  {.__vptr = (void*)&compare_vptr_list[5]},
    {.__vptr = (void*)&compare_vptr_list[6]},  {.__vptr = (void*)&compare_vptr_list[7]},
    {.__vptr = (void*)&compare_vptr_list[8]},  {.__vptr = (void*)&compare_vptr_list[9]},
    {.__vptr = (void*)&compare_vptr_list[10]}, {.__vptr = (void*)&compare_vptr_list[11]},
    {.__vptr = (void*)&compare_vptr_list[12]}};
Comparator* i8comp() { return &comp_list[0]; }
Comparator* u8comp() { return &comp_list[1]; }
Comparator* i16comp() { return &comp_list[2]; }
Comparator* u16comp() { return &comp_list[3]; }
Comparator* i32comp() { return &comp_list[4]; }
Comparator* u32comp() { return &comp_list[5]; }
Comparator* f32comp() { return &comp_list[6]; }
Comparator* i64comp() { return &comp_list[7]; }
Comparator* u64comp() { return &comp_list[8]; }
Comparator* f64comp() { return &comp_list[9]; }
Comparator* scomp() { return &comp_list[10]; }
Comparator* pcomp() { return &comp_list[11]; }
Comparator* stringcomp() { return &comp_list[12]; }
