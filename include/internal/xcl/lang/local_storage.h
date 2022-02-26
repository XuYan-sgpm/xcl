//
// Created by 徐琰 on 2022/2/19.
//

#ifndef XCL_SRC_XCL_BUILTIN_LANG_LOCAL_STORAGE_H_
#define XCL_SRC_XCL_BUILTIN_LANG_LOCAL_STORAGE_H_

#include <atomic>
namespace xcl::local_storage {
class LocalStorage {
public:
  void *GetObject(unsigned index) const;
  void PutObject(unsigned index, void *object);
  static unsigned GenerateId();

public:
  LocalStorage() = default;
  LocalStorage(const LocalStorage &) = delete;
  ~LocalStorage();

private:
  static unsigned Grow(unsigned old, unsigned expect);
  unsigned GrowStorage(unsigned index) const;

private:
  void **local_objects_ = nullptr;
  unsigned cap_ = 0;
  static const unsigned initial_cap_;
  static std::atomic<unsigned> atomic_id_;
};
} // namespace xcl::local_storage

#endif // XCL_SRC_XCL_BUILTIN_LANG_LOCAL_STORAGE_H_
