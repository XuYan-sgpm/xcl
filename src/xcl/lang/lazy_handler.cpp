//
// Created by 徐琰 on 2021/10/15.
//

#include <xcl/lang/lazy_handler.h>

namespace xcl {
LazyHandler::LazyHandler(func_t f, const Args &args) : pair_(f, args) {}
LazyHandler::~LazyHandler() = default;
LazyHandler::Pair::~Pair() {
  if (func) {
    func(args);
  }
}
LazyHandler::Pair::Pair(LazyHandler::func_t f, const Args &args)
    : func(f), args(args) {}
} // namespace xcl
