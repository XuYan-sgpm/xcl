//
// Created by 徐琰 on 2021/12/12.
//

#include <cstdarg>
#include <cstdlib>
#include <xcl/lang/heap_args.h>

unsigned int HeapArgs::nArgs() const { return count; }

void *HeapArgs::getArg(unsigned int idx) const { return nullptr; }

HeapArgs::HeapArgs(unsigned int n, ...) {
  if (n > 0) {
    cap = n;
    ptrValues = (void **)malloc(sizeof(void *) * cap);
    va_list argList;
    va_start(argList, n);
    for (unsigned i = 0; i < cap; ++i) {
      ptrValues[i] = va_arg(argList, void *);
    }
    va_end(argList);
    count = cap;
  }
}

HeapArgs::HeapArgs(const HeapArgs &heapArgs) {
  if (heapArgs.count > 0) {
    count = heapArgs.count;
    cap = count;
    ptrValues = (void **)malloc(sizeof(void *) * cap);
    for (unsigned i = 0; i < cap; i++) {
      ptrValues[i] = heapArgs.ptrValues[i];
    }
  }
}

HeapArgs::HeapArgs(HeapArgs &&heapArgs) noexcept {
  count = heapArgs.count;
  cap = heapArgs.cap;
  ptrValues = heapArgs.ptrValues;
  heapArgs.cap = heapArgs.count = 0;
  heapArgs.ptrValues = nullptr;
}

HeapArgs::~HeapArgs() {
  if (ptrValues) {
    cap = count = 0;
    free(ptrValues);
    ptrValues = nullptr;
  }
}

bool HeapArgs::pushArg(void *arg) {
  if (count < cap) {
    ptrValues[count++] = arg;
    return true;
  } else {
    return false;
  }
}

void *HeapArgs::popArg(unsigned int idx) const {
  if (idx >= 0 && idx < count) {
    return ptrValues[idx];
  } else {
    throw;
  }
}

HeapArgs::Slice HeapArgs::push(void *arg) {
  if (!pushArg(arg)) {
    throw;
  }
  return {*this, count};
}

HeapArgs::Slice HeapArgs::pop(void *&arg) {
  arg = popArg(0);
  return {*this, 1};
}

HeapArgs::Slice::Slice(HeapArgs &heapArgs) : heapArgs(heapArgs) {}

HeapArgs::Slice &HeapArgs::Slice::push(void *arg) {
  if (!heapArgs.pushArg(arg)) {
    throw;
  }
  return *this;
}

HeapArgs::Slice &HeapArgs::Slice::pop(void *&arg) {
  if (cur < heapArgs.count) {
    arg = heapArgs.popArg(cur++);
  } else {
    throw;
  }
  return *this;
}

HeapArgs::Slice::Slice(HeapArgs &heapArgs, unsigned int cur)
    : heapArgs(heapArgs), cur(cur) {}

HeapArgs::Slice::Slice(const HeapArgs::Slice &slice) = default;
