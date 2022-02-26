//
// Created by 徐琰 on 2021/12/31.
//

#ifndef SCL_COMPARATOR_H
#define SCL_COMPARATOR_H

namespace xcl {
template <typename E> class Comparator {
public:
  virtual int operator()(const E &e1, const E &e2) const = 0;

public:
  virtual ~Comparator() = default;
};
} // namespace xcl

#endif // SCL_COMPARATOR_H
