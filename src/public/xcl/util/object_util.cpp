//
// Created by 徐琰 on 2021/12/20.
//

#include <xcl/util/object_util.h>
void *
ObjectUtil::CheckNotNull(void *p, const char *name) {
  if (!p) { throw; }
  return p;
}

int
ObjectUtil::CheckPositive(int val, const char *name) {
  if (val <= 0) { throw; }
  return val;
}

long
ObjectUtil::CheckPositive(long val, const char *name) {
  if (val <= 0) { throw; }
  return val;
}

int
ObjectUtil::CheckPositiveOrZero(int val, const char *name) {
  if (val < 0) { throw; }
  return val;
}

long
ObjectUtil::CheckPositiveOrZero(long val, const char *name) {
  if (val < 0) { throw; }
  return val;
}

int
ObjectUtil::CheckInRange(int val, int start, int finish, const char *name) {
  if (finish < start || val < start || val >= finish) { throw; }
  return val;
}

long
ObjectUtil::CheckInRange(long val, long start, long finish, const char *name) {
  if (finish < start || val < start || val >= finish) { throw; }
  return val;
}
