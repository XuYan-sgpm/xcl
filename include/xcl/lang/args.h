//
// Created by 徐琰 on 2021/12/11.
//

#ifndef SCL_ARGS_H
#define SCL_ARGS_H

#include <cstdarg>

class Args {
public:
  class Slice final {
  private:
    const Args &args;
    unsigned cur = 0;

  public:
    Slice(const Args &args, unsigned cur = 0);
    Slice(const Slice &slice);

  public:
    Slice &prev(unsigned n = 1);
    Slice &next(unsigned n = 1);
    Slice &get(void *&arg);
    Slice copy() const;
  };

public:
  virtual unsigned nArgs() const = 0;
  virtual void *getArg(unsigned idx) const = 0;

public:
  virtual ~Args() = default;
};

#endif // SCL_ARGS_H
