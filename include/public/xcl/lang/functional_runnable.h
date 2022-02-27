//
// Created by 徐琰 on 2022/2/18.
//

#ifndef XCL_SRC_XCL_PUBLIC_LANG_FUNCTIONAL_RUNNABLE_H_
#define XCL_SRC_XCL_PUBLIC_LANG_FUNCTIONAL_RUNNABLE_H_

#include "runnable.h"
namespace xcl {
class FunctionalRunnable : public Runnable {
 public:
  void
  Run() override;

 public:
  FunctionalRunnable(void *(*func)(void *), void *arg);

 public:
  void *
  result() const;

 private:
  void *(*func_)(void *) = nullptr;
  void *arg_ = nullptr;
  void *result_ = nullptr;
};
}// namespace xcl

#endif// XCL_SRC_XCL_PUBLIC_LANG_FUNCTIONAL_RUNNABLE_H_
