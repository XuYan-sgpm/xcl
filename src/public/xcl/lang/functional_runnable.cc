//
// Created by 徐琰 on 2022/2/18.
//

#include <xcl/lang/functional_runnable.h>
namespace xcl {
void FunctionalRunnable::Run() {
  if (func_) {
    result_ = func_(arg_);
  }
}
FunctionalRunnable::FunctionalRunnable(void *(*func)(void *), void *arg) {
  func_ = func;
  arg_ = arg;
}
void *FunctionalRunnable::result() const { return result_; }
} // namespace xcl
