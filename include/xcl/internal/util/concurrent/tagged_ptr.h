//
// Created by 徐琰 on 2022/2/8.
//

#ifndef SCL_SRC_BUILTIN_SCL_UTIL_CONCURRENT_TAGGED_PTR_H_
#define SCL_SRC_BUILTIN_SCL_UTIL_CONCURRENT_TAGGED_PTR_H_

namespace xcl {
class TaggedPtr {
  void *ptr_ = nullptr;
  unsigned long tag_ = 0;

public:
  TaggedPtr() = default;
  TaggedPtr(void *p, unsigned long tag);

public:
  void *ptr() const;
  void set_ptr(void *p);
  unsigned long tag() const;
  void set_tag(unsigned long tag);
  bool operator==(const TaggedPtr &tagged_ptr) const;
  bool operator!=(const TaggedPtr &tagged_ptr) const;
  operator bool() const;
  TaggedPtr New(void *p) const;
  TaggedPtr GetNextPtr() const;
  void SetToNext();
};
} // namespace xcl

#endif // SCL_SRC_BUILTIN_SCL_UTIL_CONCURRENT_TAGGED_PTR_H_
