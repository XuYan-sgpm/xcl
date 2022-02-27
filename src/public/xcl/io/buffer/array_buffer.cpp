//
// Created by 徐琰 on 2021/10/11.
//

#include <xcl/io/buffer/array_buffer.h>

#include <cstring>

ArrayBuffer::ArrayBuffer(int c) {
  buf = new char[c];
  cap = c;
}

ArrayBuffer::ArrayBuffer(const ArrayBuffer &ab) : ArrayBuffer(ab.cap) {
  if (ab.finish - ab.start >= 0) {
	memcpy(buf, ab.at(0), ab.size());
  } else {
	int right = ab.cap - ab.start;
	memcpy(buf, ab.at(0), right);
	memcpy(buf + right, ab.buf, ab.size() - right);
  }
  finish = start + ab.size();
}

ArrayBuffer::ArrayBuffer(ArrayBuffer &&ab) noexcept {
  buf = ab.buf;
  start = ab.start;
  finish = ab.finish;
  cap = ab.cap;
  ab.~ArrayBuffer();
}

ArrayBuffer::~ArrayBuffer() {
  buf = nullptr;
  start = finish = 0;
  cap = 0;
}

int
ArrayBuffer::position(int idx) const {
  int absIdx = idx + start;
  return absIdx >= 0      ? absIdx % cap
		 : absIdx >= -cap ? cap + absIdx
						  : cap - (-absIdx) % cap;
}

char *
ArrayBuffer::at(int idx) const {
  return buf + position(idx);
}

int
ArrayBuffer::next(int idx, int step) const {
  return position(idx + step);
}

int
ArrayBuffer::capacity() const {
  return cap;
}

int
ArrayBuffer::size() const {
  int sub = finish - start;
  return sub >= 0 ? sub : cap - start + finish;
}

bool
ArrayBuffer::empty() const {
  return finish == start;
}

bool
ArrayBuffer::add(int idx, char ch) {
  if (reserve(idx, 1)) {
	*at(idx) = ch;
	return true;
  }
  return false;
}

bool
ArrayBuffer::reserve(int idx, int n) {
  if (checkAdd(idx, n)) {
	int s = size();
	if ((idx << 1) < s) {
	  move(-n, 0, idx);
	  start = (start - n + cap) % cap;
	} else {
	  move(idx + n, idx, s - idx);
	  finish = (finish + n) % cap;
	}
	return true;
  }
  return false;
}

void
ArrayBuffer::move(int dstIdx, int srcIdx, int n) {
  if (n > 0 && dstIdx != srcIdx) {
	int i, j;
	int cl;
	if (dstIdx < srcIdx) {
	  i = position(dstIdx);
	  j = position(srcIdx);
	  while (n > 0) {
		int l1 = cap - i > n ? n : cap - i;
		int l2 = cap - j > n ? n : cap - j;
		cl = l1 > l2 ? l2 : l1;
		memcpy(buf + i, buf + j, cl);
		n -= cl;
		i = (i + cl) % cap;
		j = (j + cl) % cap;
	  }
	} else {
	  i = position(dstIdx + n);
	  j = position(srcIdx + n);
	  while (n > 0) {
		i = (i + cap - 1) % cap + 1;
		j = (j + cap - 1) % cap + 1;
		int l1 = i > n ? n : i;
		int l2 = j > n ? n : j;
		cl = l1 > l2 ? l2 : l1;
		memmove(buf + i - cl, buf + j - cl, cl);
		n -= cl;
		i -= cl;
		j -= cl;
	  }
	}
  }
}

void
ArrayBuffer::fill(int dstIdx, int n, char ch) {
  if (n > 0) {
	int i = position(dstIdx);
	int len;
	while (n > 0) {
	  len = cap - i > n ? n : cap - i;
	  char *p = buf + i;
	  i = (i + len) % cap;
	  for (; len > 0; --len, --n) { *p++ = ch; }
	}
  }
}

void
ArrayBuffer::copy(int dstIdx, char *src, int n) {
  if (n > 0) {
	int i = position(dstIdx);
	int len;
	while (n > 0) {
	  len = cap - i > n ? n : cap - i;
	  char *p = buf + i;
	  memcpy(p, src, len);
	  src += len;
	  i = (i + len) % cap;
	  n -= len;
	}
  }
}

bool
ArrayBuffer::add(int idx, int n, char ch) {
  if (reserve(idx, n)) {
	fill(idx, n, ch);
	return true;
  }
  return false;
}

bool
ArrayBuffer::add(int idx, char *src, int n) {
  if (reserve(idx, n)) {
	copy(idx, src, n);
	return true;
  }
  return false;
}

bool
ArrayBuffer::add(int idx, const Buffer &buffer, int offset, int n) {
  if (&buffer == this) {
	if (idx == offset) { return true; }
	if (reserve(idx, n)) {
	  move(idx, offset, n);
	  return true;
	}
	return false;
  }
  if (reserve(idx, n)) {
	int i = position(idx);
	const ArrayBuffer *pArrBuf;
	if ((pArrBuf = dynamic_cast<const ArrayBuffer *>(&buffer)) == nullptr) {
	  for (; n > 0; --n) {
		*(buf + i) = *buffer.at(offset++);
		i = i < cap - 1 ? i + 1 : 0;
	  }
	} else {
	  int j = pArrBuf->position(offset);
	  int cpyLen;
	  while (n > 0) {
		int l1 = cap - i > n ? n : cap - i;
		int l2 = pArrBuf->cap - j > n ? n : pArrBuf->cap - j;
		cpyLen = l1 > l2 ? l2 : l1;
		memcpy(buf + i, pArrBuf->buf + j, cpyLen);
		n -= cpyLen;
		i = (i + cpyLen) % cap;
		j = (j + cpyLen) % (pArrBuf->cap);
	  }
	}
	return true;
  }
  return false;
}

bool
ArrayBuffer::del(int idx) {
  if (checkIndex(idx)) {
	int s = size();
	if ((idx << 1) < s) {
	  move(1, 0, idx);
	  start = (start + 1) % cap;
	} else {
	  move(idx, idx + 1, s - idx - 1);
	  finish = (finish - 1 + cap) % cap;
	}
	return true;
  }
  return false;
}

bool
ArrayBuffer::del(int fromIdx, int n) {
  if (checkRange(fromIdx, n)) {
	int s = size();
	if ((fromIdx << 1) < s - n) {
	  move(n, 0, fromIdx);
	  start = (start + n) % cap;
	} else {
	  move(fromIdx, fromIdx + n, s - fromIdx - n);
	  finish = (finish - n + cap) % cap;
	}
	return true;
  }
  return false;
}

void
ArrayBuffer::wrap(char *src, int n) {
  buf = src;
  cap = n;
  start = 0;
  finish = start + n;
}

bool
ArrayBuffer::resize(int n) {
  int s = size();
  if (checkAdd(s, n - s)) {
	if (n == s) { return true; }
	if (n < s) {
	  finish = (finish + cap - (s - n)) % cap;
	} else {
	  finish = (finish + (n - s)) % cap;
	}
	return true;
  }
  return false;
}

bool
ArrayBuffer::reserve(int n) {
  return checkAdd(size(), n - size());
}
