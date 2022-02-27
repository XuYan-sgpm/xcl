//
// Created by 徐琰 on 2022/1/3.
//

#ifndef SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_SIMPLE_OBJECT_HASH_MAP_H_
#define SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_SIMPLE_OBJECT_HASH_MAP_H_

#include <shared_mutex>

namespace xcl {
class SimpleObjectHashMap {
 public:
  SimpleObjectHashMap();
  ~SimpleObjectHashMap();

 public:
  void
  Put(void *key, void *value);
  void *
  Get(void *key) const;
  unsigned
  Size() const;

 private:
  void
  Resize();

 private:
  void *buckets_;
  unsigned size_ = 0;
  unsigned cap_ = 0;
};
}// namespace xcl

#endif// SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_SIMPLE_OBJECT_HASH_MAP_H_
