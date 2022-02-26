//
// Created by 徐琰 on 2022/1/10.
//

#ifndef SCL_SCL_UTIL_CONTAINER_OBJECTS_HANDLER_H_
#define SCL_SCL_UTIL_CONTAINER_OBJECTS_HANDLER_H_

#include "plain_iterable.h"

namespace xcl {
class ObjectsHandler {
public:
  virtual ~ObjectsHandler() = default;

public:
  virtual void *Copy(void *dst, const void *first, const void *last) = 0;
  virtual void *Copy(void *dst, void *end, const void *first,
                     const void *last) = 0;
  virtual void *Move(void *dst, void *first, void *last) = 0;
  virtual void *Move(void *dst, void *end, void *first, void *last) = 0;
  virtual void *Fill(void *dst, unsigned n, const void *value) = 0;
  virtual void *Fill(void *first, void *last, const void *value) = 0;
  virtual void *UninitializedCopy(void *dst, const void *first,
                                  const void *last) = 0;
  virtual void *UninitializedCopy(void *dst, void *end, const void *first,
                                  const void *last) = 0;
  virtual void *UninitializedMove(void *dst, void *first, void *last) = 0;
  virtual void *UninitializedMove(void *dst, void *end, void *first,
                                  void *last) = 0;
  virtual void *UninitializedFill(void *dst, unsigned n, const void *value) = 0;
  virtual void *UninitializedFill(void *first, void *last,
                                  const void *value) = 0;
  virtual void *Destroy(void *first, void *last) = 0;

  virtual void Copy(PlainIterable *dst, PlainIterable *first,
                    const PlainIterable &last) = 0;
  virtual void Copy(PlainIterable *dst, const PlainIterable &end,
                    PlainIterable *first, const PlainIterable &last) = 0;
  virtual void Move(PlainIterable *dst, PlainIterable *first,
                    const PlainIterable &last) = 0;
  virtual void Move(PlainIterable *dst, const PlainIterable &end,
                    PlainIterable *first, const PlainIterable &last) = 0;
  virtual void Fill(PlainIterable *dst, unsigned n, const void *value) = 0;
  virtual void Fill(PlainIterable *first, const PlainIterable &last,
                    const void *value) = 0;
  virtual void UninitializedCopy(PlainIterable *dst, PlainIterable *first,
                                 const PlainIterable &last) = 0;
  virtual void UninitializedCopy(PlainIterable *dst, const PlainIterable &end,
                                 PlainIterable *first,
                                 const PlainIterable &last) = 0;
  virtual void UninitializedMove(PlainIterable *dst, PlainIterable *first,
                                 const PlainIterable &last) = 0;
  virtual void UninitializedMove(PlainIterable *dst, const PlainIterable &end,
                                 PlainIterable *first,
                                 const PlainIterable &last) = 0;
  virtual void UninitializedFill(PlainIterable *dst, unsigned n,
                                 const void *value) = 0;
  virtual void UninitializedFill(PlainIterable *first,
                                 const PlainIterable &last,
                                 const void *value) = 0;
  virtual void Destroy(PlainIterable *first, const PlainIterable &last) = 0;
};
} // namespace xcl

#endif // SCL_SCL_UTIL_CONTAINER_OBJECTS_HANDLER_H_
