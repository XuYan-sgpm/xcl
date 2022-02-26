//
// Created by 徐琰 on 2022/2/18.
//

#ifndef SCL_SCL_LANG_THREAD_LOCAL_H_
#define SCL_SCL_LANG_THREAD_LOCAL_H_

#include "object.h"
namespace xcl {
class ThreadLocal final : public Object {
public:
  void *Get() const;
  void Set(void *object);

public:
  ThreadLocal();
  ThreadLocal(const ThreadLocal &) = delete;

public:
  ThreadLocal &operator=(const ThreadLocal &) = delete;

private:
  const unsigned id_;

protected:
  void *operator new(size_t);
  void operator delete(void *p);
};
} // namespace xcl

#endif // SCL_SCL_LANG_THREAD_LOCAL_H_
