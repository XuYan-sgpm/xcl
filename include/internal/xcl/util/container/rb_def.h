//
// Created by 徐琰 on 2021/12/26.
//

#ifndef SCL_RB_DEF_H
#define SCL_RB_DEF_H

namespace xcl {
enum Color { RED, BLACK };

struct RbNode {
  RbNode *left, *right, *par;
  Color color;
};
}// namespace xcl

#endif// SCL_RB_DEF_H
