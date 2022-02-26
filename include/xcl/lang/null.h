//
// Created by 徐琰 on 2021/10/19.
//

#ifndef SCL_NULL_H
#define SCL_NULL_H

class Null final {
private:
  Null() = default;

private:
  static Null *pInstance;

public:
  const static Null &get();
};

#define null Null::get()

#endif // SCL_NULL_H
