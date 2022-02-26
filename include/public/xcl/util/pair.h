//
// Created by 徐琰 on 2021/12/30.
//

#ifndef SCL_PAIR_H
#define SCL_PAIR_H

#include <type_traits>

using namespace std;

namespace xcl {
template <typename First, typename Second> struct Pair {
public:
  const First first;
  const Second second;

public:
  Pair();

  Pair(const First &f, const Second &s);

  Pair(First &&f, Second &&s);

  Pair(const Pair &p);

  Pair(Pair &&p) noexcept;

  ~Pair();
};

template <typename First, typename Second>
Pair<First, Second>::Pair() : first(), second() {}

template <typename First, typename Second>
Pair<First, Second>::Pair(const First &f, const Second &s)
    : first(f), second(s) {}

template <typename First, typename Second>
Pair<First, Second>::Pair(First &&f, Second &&s)
    : first(std::move(f)), second(std::move(s)) {}

template <typename First, typename Second>
Pair<First, Second>::Pair(const Pair &p) : first(p.first), second(p.second) {}

template <typename First, typename Second>
Pair<First, Second>::Pair(Pair &&p) noexcept
    : first(std::move(p.first)), second(std::move(p.second)) {}

template <typename First, typename Second> Pair<First, Second>::~Pair() {
  if (!is_trivially_destructible_v<First>) {
    first.~First();
  }
  if (!is_trivially_destructible_v<Second>) {
    second.~Second();
  }
}
} // namespace xcl

#endif // SCL_PAIR_H
