//
// Created by 徐琰 on 2021/10/11.
//

#ifndef SCL_BUFFER_H
#define SCL_BUFFER_H

class Buffer {
public:
  virtual int capacity() const = 0;

  virtual int size() const = 0;

  virtual bool empty() const = 0;

  virtual bool push(char ch);

  virtual bool push(int n, char ch);

  virtual bool push(char *src, int n);

  virtual bool push(const Buffer &buffer);

  virtual bool push(const Buffer &buffer, int n);

  virtual bool push(const Buffer &buffer, int offset, int n);

  virtual bool add(int idx, char ch) = 0;

  virtual bool add(int idx, int n, char ch) = 0;

  virtual bool add(int idx, char *src, int n) = 0;

  virtual bool add(int idx, const Buffer &buffer, int n);

  virtual bool add(int idx, const Buffer &buffer, int offset, int n) = 0;

  virtual bool add(int idx, const Buffer &buffer);

  virtual bool del(int idx) = 0;

  virtual bool del(int fromIdx, int n) = 0;

  virtual void wrap(char *src, int n) = 0;

  virtual char *at(int idx) const = 0;

  virtual bool reserve(int n) = 0;

  virtual bool resize(int n) = 0;

  virtual bool resize(int n, char ch);

public:
  virtual ~Buffer() = default;

protected:
  bool checkIndex(int idx) const;

  bool checkAdd(int idx, int n) const;

  bool checkDel(int idx, int n) const;

  bool checkRange(int idx, int n) const;
};

#endif // SCL_BUFFER_H
