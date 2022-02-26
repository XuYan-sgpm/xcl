//
// Created by 徐琰 on 2021/10/15.
//

#ifndef SCL_STRING_H
#define SCL_STRING_H

template <typename CharType> class CharSequence final {
public:
  CharSequence();

  CharSequence(int c);

  CharSequence(const CharSequence &seq);

  CharSequence(CharSequence &&seq);

  ~CharSequence();

private:
  struct Data {
    CharType *ptr = nullptr;

    CharType cache[16];

    ~Data();
  };
};

class String {};

#endif // SCL_STRING_H
