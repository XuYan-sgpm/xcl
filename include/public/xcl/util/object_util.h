//
// Created by 徐琰 on 2021/12/20.
//

#ifndef SCL_OBJECT_UTIL_H
#define SCL_OBJECT_UTIL_H

class ObjectUtil {
 private:
  ObjectUtil() = default;

 public:
  static void *
  CheckNotNull(void *p, const char *name);

  static int
  CheckPositive(int val, const char *name);

  static long
  CheckPositive(long val, const char *name);

  static int
  CheckPositiveOrZero(int val, const char *name);

  static long
  CheckPositiveOrZero(long val, const char *name);

  static int
  CheckInRange(int val, int start, int finish, const char *name);

  static long
  CheckInRange(long val, long start, long finish, const char *name);
};

#endif// SCL_OBJECT_UTIL_H
