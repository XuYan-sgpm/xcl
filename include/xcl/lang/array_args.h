//
// Created by 徐琰 on 2021/12/11.
//

#ifndef SCL_ARRAYARGS_H
#define SCL_ARRAYARGS_H

template <unsigned NARGS> class ArrayArgs;

template <> class ArrayArgs<0> : public Args {
public:
  unsigned int nArgs() const override { return 0; }

  void *getArg(unsigned int idx) const override { return nullptr; }
};

template <unsigned NARGS> class ArrayArgs : public Args {
  static_assert(NARGS > 0, "args must > 0");

private:
  void *a[NARGS] = {nullptr};

public:
  unsigned int nArgs() const override { return NARGS; }

  void *getArg(unsigned int idx) const override { return a[idx]; }

public:
  ArrayArgs(unsigned n, ...) {
    if (n != NARGS) {
      throw;
    }
    va_list l;
    va_start(l, n);
    for (unsigned i = 0; i < n; i++) {
      a[i] = va_arg(l, void *);
    }
    va_end(l);
  }

public:
  void initialize(unsigned n, va_list l) {
    if (n != NARGS) {
      throw;
    }
    for (unsigned i = 0; i < n; i++) {
      a[i] = va_arg(l, void *);
    }
  }
};

#endif // SCL_ARRAYARGS_H
