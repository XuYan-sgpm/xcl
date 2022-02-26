//
// Created by 徐琰 on 2022/2/10.
//

#include <xcl/lang/object.h>
namespace xcl {
long Object::HashCode() const { return (long)this; }
bool Object::Equals(const Object &o) const { return &o == this; }
} // namespace xcl