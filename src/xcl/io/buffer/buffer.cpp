//
// Created by 徐琰 on 2021/10/11.
//

#include <cstring>
#include <xcl/io/buffer/buffer.h>

bool Buffer::checkIndex(int idx) const { return idx >= 0 && idx < size(); }

bool Buffer::checkAdd(int idx, int n) const {
  return idx >= 0 && n >= 0 && size() + n <= capacity();
}

bool Buffer::checkDel(int idx, int n) const { return checkRange(idx, n); }

bool Buffer::checkRange(int idx, int n) const {
  return idx >= 0 && n >= 0 && idx + n <= size();
}

bool Buffer::push(char ch) { return add(size(), ch); }

bool Buffer::push(int n, char ch) { return add(size(), n, ch); }

bool Buffer::push(char *src, int n) { return add(size(), src, n); }

bool Buffer::push(const Buffer &buffer) { return add(size(), buffer); }

bool Buffer::push(const Buffer &buffer, int n) {
  return add(size(), buffer, n);
}

bool Buffer::push(const Buffer &buffer, int offset, int n) {
  return add(size(), buffer, offset, n);
}

bool Buffer::add(int idx, const Buffer &buffer, int n) {
  return add(idx, buffer, 0, n);
}

bool Buffer::add(int idx, const Buffer &buffer) {
  return add(idx, buffer, 0, buffer.size());
}

bool Buffer::resize(int n, char ch) {
  int s = size();
  if (n == s) {
    return true;
  }
  if (n < s) {
    del(n, s - n);
    return true;
  }
  return add(s, n - s, ch);
}
