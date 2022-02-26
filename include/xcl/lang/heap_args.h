//
// Created by 徐琰 on 2021/12/12.
//

#ifndef SCL_HEAPARGS_H
#define SCL_HEAPARGS_H

#include "args.h"
class HeapArgs : public Args {
public:
  unsigned int nArgs() const override;

  void *getArg(unsigned int idx) const override;

private:
  void **ptrValues = nullptr;

  unsigned cap = 0;

  unsigned count = 0;

public:
  class Slice final {
  private:
    HeapArgs &heapArgs;

    unsigned cur = 0;

  public:
    Slice(HeapArgs &heapArgs);

    Slice(HeapArgs &heapArgs, unsigned cur);

    Slice(const Slice &slice);

  public:
    Slice &push(void *arg);

    Slice &pop(void *&arg);

    Slice &prev();

    Slice &next();

    Slice &prev(unsigned n);

    Slice &next(unsigned n);

    Slice &set(void *arg);

    Slice &get(void *&arg);
  };

public:
  HeapArgs() = default;

  HeapArgs(unsigned n, ...);

  HeapArgs(const HeapArgs &heapArgs);

  HeapArgs(HeapArgs &&heapArgs) noexcept;

  ~HeapArgs() override;

protected:
  bool pushArg(void *arg);

  void *popArg(unsigned idx) const;

public:
  Slice push(void *arg);

  Slice pop(void *&arg);
};

#endif // SCL_HEAPARGS_H
