//
// Created by 徐琰 on 2021/10/15.
//

#ifndef SCL_LAZY_HANDLER_H
#define SCL_LAZY_HANDLER_H

#include "args.h"

namespace xcl {
class LazyHandler final {
 public:
  typedef void (*func_t)(const Args &);

 private:
  struct Pair {
	func_t const func;
	const Args &args;

   public:
	Pair(func_t f, const Args &args);
	~Pair();
  };
  Pair pair_;

 public:
  LazyHandler(func_t f, const Args &args);
  ~LazyHandler();
};
}// namespace xcl

#endif// SCL_LAZY_HANDLER_H
