//
// Created by 徐琰 on 2022/2/9.
//

#include <xcl/lang/object.h>
#include <xcl/util/pipeline.h>
namespace xcl {
void Pipeline::Add(Functional *functional) {
  auto node = new Node();
  node->functional = functional;
  if (!first_) {
    first_ = last_ = node;
  } else {
    last_->next = node;
    node->prev = last_;
    last_ = node;
  }
}
Object *Pipeline::Fire(bool reverse) { return Fire(nullptr, reverse); }
Pipeline::~Pipeline() {
  auto cur_node = first_;
  while (cur_node) {
    auto next_node = cur_node->next;
    delete cur_node;
    cur_node = next_node;
  }
  first_ = last_ = nullptr;
}
void Pipeline::AddFirst(Functional *functional) {
  auto node = new Node();
  node->functional = functional;
  node->next = first_;
  first_ = node;
  if (!last_) {
    last_ = first_;
  }
}
void Pipeline::Add(const Pipeline &pipeline) {
  auto src_node = pipeline.first_;
  if (!src_node) {
    return;
  }
  Add(src_node->functional);
  while ((src_node = src_node->next)) {
    auto node = new Node();
    node->functional = src_node->functional;
    last_->next = node;
    node->prev = last_;
    last_ = node;
  }
}
void Pipeline::Add(Pipeline &&pipeline) {
  last_->next = pipeline.first_;
  pipeline.first_->prev = last_;
  last_ = pipeline.last_;
  pipeline.first_ = pipeline.last_ = nullptr;
}
void Pipeline::AddFirst(const Pipeline &pipeline) {
  Pipeline tmp_pipeline(pipeline);
  AddFirst(static_cast<Pipeline &&>(tmp_pipeline));
}
void Pipeline::AddFirst(Pipeline &&pipeline) {
  pipeline.last_->next = first_;
  first_->prev = pipeline.last_;
  first_ = pipeline.first_;
  pipeline.first_ = pipeline.last_ = nullptr;
}
Pipeline::Pipeline(const Pipeline &pipeline) { Add(pipeline); }
Pipeline::Pipeline(Pipeline &&pipeline) noexcept {
  Add(static_cast<Pipeline &&>(pipeline));
}
Object *Pipeline::Fire(Object *arg, bool reverse) {
  Object *result = arg;
  if (!reverse) {
    auto cur_node = first_;
    while (cur_node) {
      result = cur_node->functional->Invoke(result);
      cur_node = cur_node->next;
    }
  } else {
    auto cur_node = last_;
    while (cur_node) {
      result = cur_node->functional->Invoke(result);
      cur_node = cur_node->prev;
    }
  }
  return result;
}
} // namespace xcl
