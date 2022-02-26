//
// Created by 徐琰 on 2022/2/7.
//

#ifndef SCL_SCL_LANG_OBJECT_H_
#define SCL_SCL_LANG_OBJECT_H_

namespace xcl {
class Object {
public:
  virtual ~Object() = default;

public:
  virtual long HashCode() const;
  virtual bool Equals(const Object &o) const;
};
} // namespace xcl

#endif // SCL_SCL_LANG_OBJECT_H_
