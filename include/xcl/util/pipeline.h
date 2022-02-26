//
// Created by 徐琰 on 2022/2/9.
//

#ifndef SCL_SCL_UTIL_PIPELINE_H_
#define SCL_SCL_UTIL_PIPELINE_H_

#include "functional.h"
namespace xcl {
class Pipeline : public Object {
  struct Node {
    Functional *functional = nullptr;
    Node *next = nullptr;
    Node *prev = nullptr;
  };
  Node *first_ = nullptr;
  Node *last_ = nullptr;

public:
  void Add(Functional *functional);
  void AddFirst(Functional *functional);
  void Add(const Pipeline &pipeline);
  void Add(Pipeline &&pipeline);
  void AddFirst(const Pipeline &pipeline);
  void AddFirst(Pipeline &&pipeline);
  Object *Fire(bool reverse = false);
  Object *Fire(Object *arg, bool reverse = false);

public:
  ~Pipeline() override;
  Pipeline() = default;
  Pipeline(const Pipeline &pipeline);
  Pipeline(Pipeline &&pipeline) noexcept;
};
} // namespace xcl

#endif // SCL_SCL_UTIL_PIPELINE_H_
