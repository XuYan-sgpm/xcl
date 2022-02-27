//
// Created by 徐琰 on 2021/10/11.
//

#ifndef SCL_ARRAYBUFFER_H
#define SCL_ARRAYBUFFER_H

#include "buffer.h"

class ArrayBuffer : public Buffer {
 public:
  int
  capacity() const override;

  int
  size() const override;

  bool
  empty() const override;

  bool
  add(int idx, char ch) override;

  bool
  add(int idx, int n, char ch) override;

  bool
  add(int idx, char *src, int n) override;

  bool
  add(int idx, const Buffer &buffer, int offset, int n) override;

  char *
  at(int idx) const override;

  bool
  del(int idx) override;

  bool
  del(int fromIdx, int n) override;

  void
  wrap(char *src, int n) override;

  bool
  resize(int n) override;

  bool
  reserve(int n) override;

 protected:
  int start = 0, finish = 0;

  char *buf = nullptr;

  int cap = 0;

 public:
  ArrayBuffer() = default;

  explicit ArrayBuffer(int c);

  ArrayBuffer(const ArrayBuffer &ab);

  ArrayBuffer(ArrayBuffer &&ab) noexcept;

  ~ArrayBuffer() override;

 protected:
  int
  position(int idx) const;

  int
  next(int idx, int step) const;

 private:
  void
  move(int dstIdx, int srcIdx, int n);

  void
  fill(int dstIdx, int n, char ch);

  void
  copy(int dstIdx, char *src, int n);

  bool
  reserve(int idx, int n);
};

#endif// SCL_ARRAYBUFFER_H
