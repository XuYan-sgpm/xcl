//
// Created by 徐琰 on 2021/12/10.
//

#ifndef SCL_COLLECTION_H
#define SCL_COLLECTION_H

#include "iterator_wrapper.h"

template<typename E>
class Collection {
 public:
  typedef IteratorWrapper<E> IterWrapper;

 public:
  virtual IterWrapper
  begin() = 0;

  virtual IterWrapper
  end() = 0;

  virtual bool
  Add(const E &e) = 0;

  virtual bool
  Remove(const E &e) = 0;

  virtual IterWrapper
  Remove(IterWrapper iter_wrapper) = 0;

  virtual unsigned
  Size() const = 0;
};

#endif// SCL_COLLECTION_H
