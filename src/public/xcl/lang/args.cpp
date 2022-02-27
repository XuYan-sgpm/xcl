//
// Created by 徐琰 on 2021/12/12.
//

#include <xcl/lang/args.h>

Args::Slice::Slice(const Args &args, unsigned int cur) : args(args), cur(cur) {}

Args::Slice &
Args::Slice::prev(unsigned int n) {
  if (n > cur) { throw; }
  cur -= n;
  return *this;
}

Args::Slice &
Args::Slice::next(unsigned int n) {
  if (n > args.nArgs() - cur) { throw; }
  cur += n;
  return *this;
}

Args::Slice &
Args::Slice::get(void *&arg) {
  if (cur >= args.nArgs()) { throw; }
  arg = args.getArg(cur++);
  return *this;
}

Args::Slice
Args::Slice::copy() const {
  return {args, cur};
}

Args::Slice::Slice(const Args::Slice &slice) = default;

// BasicParams::Slice
// BasicParams::Get(void *&args) const {
//     Slice slice(*this, 0);
//     slice.Get(args);
//     return slice;
// }
