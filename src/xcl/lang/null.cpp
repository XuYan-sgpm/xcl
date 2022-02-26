//
// Created by 徐琰 on 2021/10/19.
//

#include <xcl/lang/null.h>

Null *Null::pInstance = nullptr;

const Null &Null::get() {
  if (pInstance == nullptr) {
    pInstance = new Null();
  }
  return *pInstance;
}