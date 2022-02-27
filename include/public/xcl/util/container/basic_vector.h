//
// Created by 徐琰 on 2022/1/14.
//

#ifndef SCL_SRC_PUBLIC_SCL_UTIL_CONTAINER_BASIC_VECTOR_H_
#define SCL_SRC_PUBLIC_SCL_UTIL_CONTAINER_BASIC_VECTOR_H_

#include "include/xcl/util/allocator/allocator.h"
#include "objects_handler.h"

namespace xcl {
struct VectorContext;
class BasicVector {
 private:
  char data[32] = {0};

 protected:
  virtual unsigned
  GetElementSize() const = 0;
  virtual ObjectsHandler *
  GetInterface() const = 0;

 public:
  BasicVector();
  explicit BasicVector(unsigned n);
  BasicVector(unsigned n, Allocator *allocator);
  BasicVector(const BasicVector &v);
  BasicVector(BasicVector &&v) noexcept;
  virtual ~BasicVector() = default;

 public:
  BasicVector &
  operator=(const BasicVector &v);
  BasicVector &
  operator=(BasicVector &&v) noexcept;
  char *
  operator[](unsigned idx);

 public:
  bool
  PushBack(const void *value);
  bool
  PopBack();
  unsigned
  Size() const;
  bool
  IsEmpty() const;
  unsigned
  Capacity() const;
  bool
  Add(char *pos, const void *value);
  bool
  Add(char *pos, unsigned n, const void *value);
  bool
  Add(char *pos, const void *first, const void *last);
  bool
  Add(char *pos, PlainIterable *first, const PlainIterable &last);
  bool
  MoveAdd(char *pos, void *value);
  bool
  MoveAdd(char *pos, void *first, void *last);
  bool
  MoveAdd(char *pos, PlainIterable *first, const PlainIterable &last);
  char *
  Remove(char *pos);
  bool
  Remove(char *first, char *last);
  bool
  Clear();
  bool
  Assign(const void *first, const void *last);
  bool
  Assign(unsigned n, const void *value);
  bool
  Assign(PlainIterable *first, const PlainIterable &last);
  bool
  MoveAssign(void *first, void *last);
  bool
  MoveAssign(PlainIterable *first, const PlainIterable &last);
  bool
  Reserve(unsigned n);
  bool
  Resize(unsigned n, const void *value);

 protected:
  void
  Initialize(unsigned n, Allocator *allocator);
  void
  FreeVector();
  VectorContext *
  GetVectorContext();
  const VectorContext *
  GetVectorContext() const;
};
}// namespace xcl

#endif// SCL_SRC_PUBLIC_SCL_UTIL_CONTAINER_BASIC_VECTOR_H_
