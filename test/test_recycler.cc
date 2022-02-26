//
// Created by 徐琰 on 2022/2/9.
//
#include <iostream>
#include <xcl/util/object_pool/recycler.h>
#define COUT std::cout
#define ENDL std::endl
extern "C" {
class Array : public xcl::Object {
  char *block_ = nullptr;

public:
  ~Array() override {
    if (block_) {
      delete[] block_;
      block_ = nullptr;
    }
  }

  Array() { block_ = new char[128]; }
};

class ArrayFactory : public xcl::Factory<xcl::Object> {
public:
  xcl::Object *NewElement() const override { return new Array(); }
};
}
int main() {
  ArrayFactory array_factory;
  xcl::Recycler recycler(array_factory);
  std::cout << std::boolalpha << recycler.Get()->Recycle() << '\n';
}
