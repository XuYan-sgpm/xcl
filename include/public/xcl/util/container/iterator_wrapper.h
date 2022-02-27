//
// Created by 徐琰 on 2021/12/31.
//

#ifndef SCL_ITERATOR_WRAPPER_H
#define SCL_ITERATOR_WRAPPER_H

#include "include/xcl/util/factory.h"
#include "iterator.h"

template<typename E>
class IteratorWrapper {
 protected:
  Iterator<E> &iter;

 public:
  explicit IteratorWrapper(const Factory<Iterator<E>> &factory);

  virtual ~IteratorWrapper();

 public:
  E &
  operator*() const;

  E *
  operator->() const;

  IteratorWrapper &
  operator++();

  bool
  operator==(const IteratorWrapper &wrapper) const;

  bool
  operator==(const Iterator<E> &it) const;

  bool
  operator!=(const IteratorWrapper &wrapper) const;

  bool
  operator!=(const Iterator<E> &it) const;
};

template<typename E>
IteratorWrapper<E>::IteratorWrapper(const Factory<Iterator<E>> &factory)
	: iter(*factory.newElement()) {}

template<typename E>
IteratorWrapper<E>::~IteratorWrapper() {
  delete &iter;
}

template<typename E>
E &
IteratorWrapper<E>::operator*() const {
  return *iter;
}

template<typename E>
E *
IteratorWrapper<E>::operator->() const {
  return &(*iter);
}

template<typename E>
IteratorWrapper<E> &
IteratorWrapper<E>::operator++() {
  ++iter;
  return *this;
}

template<typename E>
bool
IteratorWrapper<E>::operator==(const IteratorWrapper &wrapper) const {
  return iter == wrapper.iter;
}

template<typename E>
bool
IteratorWrapper<E>::operator==(const Iterator<E> &it) const {
  return iter == it;
}

template<typename E>
bool
IteratorWrapper<E>::operator!=(const IteratorWrapper &wrapper) const {
  return iter != wrapper.iter;
}

template<typename E>
bool
IteratorWrapper<E>::operator!=(const Iterator<E> &it) const {
  return iter != it;
}

#endif// SCL_ITERATOR_WRAPPER_H
