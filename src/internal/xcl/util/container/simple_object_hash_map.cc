//
// Created by 徐琰 on 2022/1/3.
//

#include <cstdlib>
#include <xcl/util/container/list_core.h>
#include <xcl/util/container/list_def.h>
#include <xcl/util/container/rb_core.h>
#include <xcl/util/container/rb_def.h>
#include <xcl/util/container/simple_object_hash_map.h>

namespace xcl {
class Bucket {
  friend class SimpleObjectHashMap;

public:
  Bucket();
  Bucket(const Bucket &) = delete;

public:
  void PutToBucket(void *key, void *value, unsigned total_elements);
  void *GetFromBucket(void *key) const;

private:
  bool IsEmpty() const;
  void TransferToTreeMap();
  void TransferToList();

private:
  class List {
  public:
    struct Node : public ListNode {
      void *key;
      void *value;
    };

  public:
    List();
    explicit List(Node *h);
    ~List();

  public:
    bool Add(void *key, void *value);
    void *Get(void *key);
    unsigned size() const;
    bool IsEmpty() const;
    Node *First() const;

  private:
    Node *const head_;
    unsigned size_ = 0;
  };

private:
  class TreeMap {
  public:
    TreeMap();
    ~TreeMap();

  public:
    struct Node : public RbNode {
      struct Data {
        void *first;
        void *second;
      };

      Data data;
    };

  public:
    bool Add(void *key, void *value);
    void *Get(void *key);
    unsigned size() const;
    bool IsEmpty() const;
    Node *First() const;
    Node *Next(Node *node) const;

  private:
    static int Compare(void *, RbNode *node, const void *src);

  private:
    Node *const header_;
    unsigned size_ = 0;
  };

private:
  struct BucketContext {
    union Container {
      List *list_ptr = nullptr;
      TreeMap *map_ptr;
    };

    Container container;
    bool is_list = true;

  public:
    ~BucketContext();
  };

  BucketContext bucket_context_;
};

Bucket::Bucket() = default;

void Bucket::PutToBucket(void *key, void *value, unsigned total_elements) {
  if (!bucket_context_.is_list) {
    bucket_context_.container.map_ptr->Add(key, value);
  } else {
    auto *list_ptr = bucket_context_.container.list_ptr;
    if (!list_ptr) {
      list_ptr = new List();
      bucket_context_.container.list_ptr = list_ptr;
    }
    list_ptr->Add(key, value);
    if (list_ptr->size() >= 8 && total_elements >= 64) {
      TransferToTreeMap();
    }
  }
}

void *Bucket::GetFromBucket(void *key) const {
  if (bucket_context_.is_list) {
    auto *list_ptr = bucket_context_.container.list_ptr;
    if (!list_ptr) {
      return nullptr;
    }
    return list_ptr->Get(key);
  } else {
    auto *map_ptr = bucket_context_.container.map_ptr;
    return map_ptr->Get(key);
  }
}

bool Bucket::IsEmpty() const {
  if (bucket_context_.is_list) {
    auto *list_ptr = bucket_context_.container.list_ptr;
    return list_ptr == nullptr;
  } else {
    auto *map_ptr = bucket_context_.container.map_ptr;
    return map_ptr == nullptr;
  }
}

void Bucket::TransferToTreeMap() {
  auto *list_ptr = bucket_context_.container.list_ptr;
  auto *map_ptr = new TreeMap();
  bucket_context_.container.map_ptr = map_ptr;
  bucket_context_.is_list = false;
  auto *cur_list_node = list_ptr->First();
  while (cur_list_node) {
    map_ptr->Add(cur_list_node->key, cur_list_node->value);
    cur_list_node = (List::Node *)(cur_list_node->next);
  }
  delete list_ptr;
}

void Bucket::TransferToList() {
  auto *map_ptr = bucket_context_.container.map_ptr;
  auto *list_ptr = new List();
  bucket_context_.container.list_ptr = list_ptr;
  bucket_context_.is_list = true;
  auto *cur_tree_node = map_ptr->First();
  for (unsigned i = 0; i < map_ptr->size(); ++i) {
    list_ptr->Add(cur_tree_node->data.first, cur_tree_node->data.second);
    cur_tree_node = map_ptr->Next(cur_tree_node);
  }
  delete map_ptr;
}

Bucket::TreeMap::TreeMap() : header_(new Node()) {
  header_->color = RED;
  header_->left = header_->right = header_;
  header_->par = nullptr;
}

Bucket::TreeMap::~TreeMap() {
  RbClear(header_->par,
          {(void *)this, [](void *arg, RbNode *node) { free(node); }});
  delete header_;
  size_ = 0;
}

bool Bucket::TreeMap::Add(void *key, void *value) {
  Node::Data data{key, value};
  RbNode *par_or_target;
  int ret =
      RbFindFirst(&data, par_or_target, header_, {(void *)this, &Compare});
  if (ret == 0) {
    auto &d = ((Node *)par_or_target)->data;
    d.first = key;
    d.second = value;
  } else {
    auto *node = (Node *)malloc(sizeof(Node));
    if (!node) {
      return false;
    }
    auto &node_data = node->data;
    node_data.first = key;
    node_data.second = value;
    auto &child = ret == -1 ? par_or_target->left : par_or_target->right;
    RbAddNode(child, par_or_target, node, header_);
    ++size_;
  }
  return true;
}

void *Bucket::TreeMap::Get(void *key) {
  if (!size_) {
    return nullptr;
  }
  RbNode *par_or_target;
  int ret = RbFindFirst(key, par_or_target, header_, {nullptr, &Compare});
  return ret == 0 ? par_or_target : nullptr;
}

unsigned Bucket::TreeMap::size() const { return size_; }

bool Bucket::TreeMap::IsEmpty() const { return size_ == 0; }

Bucket::TreeMap::Node *Bucket::TreeMap::First() const {
  return (Node *)header_->left;
}

Bucket::TreeMap::Node *Bucket::TreeMap::Next(Node *node) const {
  if (node == header_) {
    return (Node *)header_->left;
  }
  if (node->right) {
    node = (Node *)node->right;
    while (node->left) {
      node = (Node *)node->left;
    }
    return node;
  }
  Node *par;
  while ((par = (Node *)node->par) != header_) {
    if (node == par->left) {
      break;
    }
    node = par;
  }
  return par;
}

int Bucket::TreeMap::Compare(void *, RbNode *node, const void *src) {
  auto diff =
      (long)((Node *)node)->data.first - (long)((const Node::Data *)src)->first;
  return diff > 0 ? 1 : diff < 0 ? -1 : 0;
}

Bucket::List::List() : head_(new Node()) {
  head_->next = nullptr;
  head_->prev = head_;
}

Bucket::List::List(Bucket::List::Node *h) : head_(h) {}

Bucket::List::~List() {
  auto *cur = head_->next;
  while (cur) {
    auto *cur_next = cur->next;
    free(cur);
    cur = cur_next;
  }
  delete head_;
}

bool Bucket::List::Add(void *key, void *value) {
  auto *node = (Node *)malloc(sizeof(Node));
  if (node) {
    node->key = key;
    node->value = value;
    node->next = nullptr;
    ListLink(head_->prev, node);
    head_->prev = node;
    ++size_;
  }
  return node;
}

void *Bucket::List::Get(void *key) {
  auto *cur = head_->next;
  while (cur) {
    if (((Node *)cur)->key == key) {
      return ((Node *)cur)->value;
    }
    cur = cur->next;
  }
  return nullptr;
}

unsigned Bucket::List::size() const { return size_; }

bool Bucket::List::IsEmpty() const { return size_ == 0; }

Bucket::List::Node *Bucket::List::First() const { return (Node *)head_->next; }

Bucket::BucketContext::~BucketContext() {
  if (is_list) {
    delete container.list_ptr;
    container.list_ptr = nullptr;
  } else {
    delete container.map_ptr;
    container.map_ptr = nullptr;
    is_list = true;
  }
}

SimpleObjectHashMap::SimpleObjectHashMap()
    : buckets_(new Bucket[16]), cap_(16) {}

SimpleObjectHashMap::~SimpleObjectHashMap() {
  delete[]((Bucket *)buckets_);
  buckets_ = nullptr;
  cap_ = 0;
  size_ = 0;
}

void SimpleObjectHashMap::Put(void *key, void *value) {
  if (size_ >= cap_ * 0.75) {
    Resize();
  }
  auto idx = ((long)key) & (cap_ - 1);
  ((Bucket *)buckets_)[idx].PutToBucket(key, value, size_);
  ++size_;
}

void *SimpleObjectHashMap::Get(void *key) const {
  auto idx = ((long)key) & (cap_ - 1);
  return ((Bucket *)buckets_)[idx].GetFromBucket(key);
}

void SimpleObjectHashMap::Resize() {
  auto new_cap = cap_ << 1;
  auto *new_buckets = new Bucket[new_cap];
  unsigned new_map_size = 0;
  for (unsigned i = 0; i < cap_; ++i) {
    auto *bucket = (((Bucket *)buckets_) + i);
    if (bucket->IsEmpty()) {
      continue;
    }
    bool is_list = bucket->bucket_context_.is_list;
    if (is_list) {
      auto *list_ptr = bucket->bucket_context_.container.list_ptr;
      auto *cur_list_node = list_ptr->First();
      for (; cur_list_node;
           cur_list_node = (Bucket::List::Node *)cur_list_node->next) {
        auto idx = ((long)cur_list_node->key) & (new_cap - 1);
        auto *new_bucket = new_buckets + idx;
        new_bucket->PutToBucket(cur_list_node->key, cur_list_node->value,
                                new_map_size++);
      }
    } else {
      auto *map_ptr = bucket->bucket_context_.container.map_ptr;
      auto *cur_tree_node = map_ptr->First();
      for (unsigned j = 0; j < map_ptr->size(); ++j) {
        auto idx = ((long)cur_tree_node->data.first) & (new_cap - 1);
        auto *new_bucket = new_buckets + idx;
        new_bucket->PutToBucket(cur_tree_node->data.first,
                                cur_tree_node->data.second, new_map_size++);
        cur_tree_node = map_ptr->Next(cur_tree_node);
      }
    }
  }
  delete[]((Bucket *)buckets_);
  cap_ = new_cap;
  buckets_ = new_buckets;
}

unsigned SimpleObjectHashMap::Size() const { return size_; }
} // namespace xcl
