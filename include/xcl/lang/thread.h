//
// Created by 徐琰 on 2022/2/9.
//

#ifndef SCL_SCL_LANG_THREAD_H_
#define SCL_SCL_LANG_THREAD_H_

#include "object.h"
#include "runnable.h"

namespace xcl {
class Thread : public Object {
public:
  bool IsAlive();
  bool Join();
  bool Detach();
  bool AddCallback(Runnable *runnable);

public:
  bool operator==(const Thread &thread) const;
  bool operator!=(const Thread &thread) const;

public:
  static Thread CurrentThread();
  static Thread NewThread(Runnable *runnable);

protected:
  Thread(void *context);

private:
  Thread() = default;

public:
  Thread(const Thread &) = delete;
  Thread(Thread &&thread) noexcept;
  ~Thread() override;

public:
  Thread &operator=(const Thread &) = delete;
  Thread &operator=(Thread &&thread) noexcept;

private:
  void *context_ = nullptr;
};
} // namespace xcl

#endif // SCL_SCL_LANG_THREAD_H_
