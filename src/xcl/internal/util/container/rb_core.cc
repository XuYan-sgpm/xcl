//
// Created by 徐琰 on 2021/12/25.
//

#include <xcl/internal/util/container/rb_core.h>

namespace xcl {
RbNode *RbMinimum(RbNode *node) {
  if (node) {
    while (node->left) {
      node = node->left;
    }
  }
  return node;
}

RbNode *RbMaximum(RbNode *node) {
  if (node) {
    while (node->right) {
      node = node->right;
    }
  }
  return node;
}

RbNode *RbPrev(RbNode *node, RbNode *header) {
  if (node == header) {
    return header->right;
  }
  if (node->left) {
    return RbMaximum(node->left);
  }
  RbNode *par;
  while ((par = node->par) != header) {
    if (node == par->right) {
      break;
    }
    node = par;
  }
  return par;
}

RbNode *RbNext(RbNode *node, RbNode *header) {
  if (node == header) {
    return node->left;
  }
  if (node->right) {
    return RbMinimum(node->right);
  }
  RbNode *par;
  while ((par = node->par) != header) {
    if (node == par->left) {
      break;
    }
    node = par;
  }
  return par;
}

void RbLeftRotate(RbNode *node, RbNode *&root) {
  auto *r = node->right;
  auto *rl = r->left;
  auto *par = node->par;
  node->right = rl;
  if (rl) {
    rl->par = node;
  }
  r->par = par;
  if (root == node) {
    root = r;
  } else if (node == par->left) {
    par->left = r;
  } else {
    par->right = r;
  }
  r->left = node;
  node->par = r;
}

void RbRightRotate(RbNode *node, RbNode *&root) {
  auto *l = node->left;
  auto *lr = l->right;
  auto *par = node->par;
  node->left = lr;
  if (lr) {
    lr->par = node;
  }
  if (root == node) {
    root = l;
  } else if (par->left == node) {
    par->left = l;
  } else {
    par->right = l;
  }
  l->par = par;
  l->right = node;
  node->par = l;
}

void RbAddFixup(RbNode *node, RbNode *&root) {
  node->color = RED;
  RbNode *par, *grand;
  while (root != node && (par = node->par)->color == RED) {
    grand = par->par;
    bool is_par_left = grand->left == par;
    RbNode *uncle = is_par_left ? grand->right : grand->left;
    if (uncle && uncle->color == RED) {
      par->color = BLACK;
      uncle->color = BLACK;
      grand->color = RED;
      node = grand;
    } else {
      if (is_par_left && node == par->right) {
        RbLeftRotate(par, root);
        par = node;
      } else if (!is_par_left && node == par->left) {
        RbRightRotate(par, root);
        par = node;
      }
      par->color = BLACK;
      grand->color = RED;
      is_par_left ? RbRightRotate(grand, root) : RbLeftRotate(grand, root);
    }
  }
  root->color = BLACK;
}

void RbRemoveFixup(RbNode *node, RbNode *par, RbNode *&root) {
  while ((!node || node->color == BLACK) && root != node) {
    bool is_node_left = node == par->left;
    RbNode *cousin = is_node_left ? par->right : par->left;
    if (is_node_left) {
      if (cousin->color == RED) {
        par->color = RED;
        cousin->color = BLACK;
        RbLeftRotate(par, root);
        cousin = par->right;
      }
      if ((!cousin->left || cousin->left->color == BLACK) &&
          (!cousin->right || cousin->right->color == BLACK)) {
        cousin->color = RED;
        node = par;
        par = node->par;
      } else {
        if (!cousin->right || cousin->right->color == BLACK) {
          cousin->color = RED;
          cousin->left->color = BLACK;
          RbRightRotate(cousin, root);
          cousin = par->right;
        }
        cousin->color = par->color;
        par->color = BLACK;
        cousin->right->color = BLACK;
        RbLeftRotate(par, root);
        break;
      }
    } else {
      if (cousin->color == RED) {
        cousin->color = BLACK;
        par->color = RED;
        RbRightRotate(par, root);
        cousin = par->left;
      }
      if ((!cousin->left || cousin->left->color == BLACK) &&
          (!cousin->right || cousin->right->color == BLACK)) {
        cousin->color = RED;
        node = par;
        par = node->par;
      } else {
        if (!cousin->left || cousin->left->color == BLACK) {
          cousin->color = RED;
          cousin->right->color = BLACK;
          RbLeftRotate(cousin, root);
          cousin = par->left;
        }
        cousin->left->color = BLACK;
        cousin->color = par->color;
        par->color = BLACK;
        RbRightRotate(par, root);
        break;
      }
    }
  }
  if (node) {
    node->color = BLACK;
  }
}

void RbAddNode(RbNode *&child, RbNode *par, RbNode *node, RbNode *header) {
  node->par = par;
  child = node;
  node->left = node->right = nullptr;
  if (par == header) {
    header->right = node;
    header->par = node;
    node->color = BLACK;
  } else {
    bool is_child_left = child == par->left;
    if (is_child_left) {
      if (par == header->left) {
        header->left = node;
      }
    } else {
      if (par == header->right) {
        header->right = node;
      }
    }
    RbAddFixup(node, header->par);
  }
}

RbNode *RbRemoveNode(RbNode *node, RbNode *header) {
  RbNode *rep = node;
  RbNode *child, *child_par;
  RbNode *par = node->par;
  auto *&root = header->par;
  if (!rep->left) {
    child = rep->right;
  } else if (!rep->right) {
    child = rep->left;
  } else {
    rep = RbMaximum(rep->left);
    child = rep->left;
  }
  if (rep != node) {
    rep->right = node->right;
    node->right->par = rep;
    if (rep == node->left) {
      child_par = rep;
    } else {
      child_par = rep->par;
      child_par->right = child;
      if (child) {
        child->par = child_par;
      }
      rep->left = node->left;
      node->left->par = rep;
    }
    if (root == node) {
      root = rep;
    } else if (node == par->left) {
      par->left = rep;
    } else {
      par->right = rep;
    }
    rep->par = par;
    auto color = node->color;
    node->color = rep->color;
    rep->color = color;
  } else {
    child_par = par;
    if (child) {
      child->par = child_par;
    }
    if (root == node) {
      root = child;
    } else if (node == par->left) {
      par->left = child;
    } else {
      par->right = child;
    }
    if (header->left == node) {
      header->left = !child ? child_par : RbMinimum(child);
    }
    if (header->right == node) {
      header->right = !child ? child_par : RbMaximum(child);
    }
  }
  if (node->color == BLACK) {
    RbRemoveFixup(child, child_par, root);
  }
  return node;
}

unsigned RbBlackCount(RbNode *node, RbNode *header) {
  unsigned c = 0;
  if (node) {
    while (node != header) {
      if (node->color == BLACK) {
        ++c;
      }
      node = node->par;
    }
  }
  return c;
}

bool RbVerify(RbNode *header,
              const rb_pair::Pair<rb_pair::NodeCompareFunc> &node_cmp_pair) {
  if (!header->par) {
    if (header->left != header || header->right != header) {
      return false;
    } else {
      return true;
    }
  }
  auto *&root = header->par;
  if (root->color == RED) {
    return false;
  }
  auto *min = header->left;
  auto *max = header->right;
  const unsigned kBlackCount = RbBlackCount(min, header);
  if (RbBlackCount(max, header) != kBlackCount) {
    return false;
  }
  auto *cur = min;
  do {
    auto *cur_left = cur->left;
    auto *cur_right = cur->right;
    if (!cur_left && !cur_right) {
      if (RbBlackCount(cur, header) != kBlackCount) {
        return false;
      }
    } else if (cur_left) {
      if (cur->color == RED && cur_left->color == RED) {
        return false;
      }
      if (node_cmp_pair.func(node_cmp_pair.args, cur, cur_left) < 0) {
        return false;
      }
    } else {
      if (cur_right->color == RED && cur->color == RED) {
        return false;
      }
      if (node_cmp_pair.func(node_cmp_pair.args, cur_right, cur) < 0) {
        return false;
      }
    }
  } while (cur != max && (cur = RbNext(cur, header)));
  if (min != RbMinimum(root)) {
    return false;
  }
  if (max != RbMaximum(root)) {
    return false;
  }
  return true;
}

RbNode *&
RbFind(const void *src, RbNode *&par, RbNode *header,
       const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair) {
  auto *&root = header->par;
  if (!root) {
    par = header;
    return par->left;
  }
  auto *cur = root;
  while (true) {
    int cmp_ret = value_cmp_pair.func(value_cmp_pair.args, cur, src);
    if (cmp_ret > 0) {
      if (cur->left) {
        cur = cur->left;
      } else {
        par = cur;
        return par->left;
      }
    } else {
      if (cur->right) {
        cur = cur->right;
      } else {
        par = cur;
        return par->right;
      }
    }
  }
}

int RbFindFirst(
    const void *src, RbNode *&par_or_target, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair) {
  auto *cur = header->par;
  if (!cur) {
    par_or_target = header;
    return -1;
  }
  while (true) {
    int cmp_ret = value_cmp_pair.func(value_cmp_pair.args, cur, src);
    if (cmp_ret == 0) {
      par_or_target = cur;
      return 0;
    }
    if (cmp_ret < 0) {
      if (cur->right) {
        cur = cur->right;
      } else {
        par_or_target = cur;
        return 1;
      }
    } else {
      if (cur->left) {
        cur = cur->left;
      } else {
        par_or_target = cur;
        return -1;
      }
    }
  }
}

RbNode *&
RbFindHint(const void *src, RbNode *&par, RbNode *hint, RbNode *header,
           const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair) {
  auto *&root = header->par;
  if (!root) {
    par = header;
    return par->left;
  }
  if (!hint) {
    hint = root;
  }
  if (hint != root &&
      (hint == header ||
       value_cmp_pair.func(value_cmp_pair.args, hint, src) >= 0)) {
    RbNode *prev;
    if (hint == header->left ||
        value_cmp_pair.func(value_cmp_pair.args, (prev = RbPrev(hint, header)),
                            src) <= 0) {
      if (!hint->left) {
        par = hint;
        return par->left;
      } else {
        par = prev;
        return par->right;
      }
    }
  }
  return RbFind(src, par, header, value_cmp_pair);
}

int RbFindFirstHint(
    const void *src, RbNode *&par_or_target, RbNode *hint, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair) {
  auto *&root = header->par;
  if (!root) {
    par_or_target = header;
    return -1;
  }
  if (!hint) {
    hint = root;
  }
  int cmp_ret = -10086;
  if (hint != root &&
      (hint == header ||
       (cmp_ret = value_cmp_pair.func(value_cmp_pair.args, hint, src) > 0))) {
    if (hint == header->left) {
      par_or_target = hint;
      return -1;
    }
    auto *prev = RbPrev(hint, header);
    if ((cmp_ret = value_cmp_pair.func(value_cmp_pair.args, prev, src)) < 0) {
      if (!hint->left) {
        par_or_target = hint;
        return -1;
      } else {
        par_or_target = prev;
        return 1;
      }
    }
    if (cmp_ret == 0) {
      par_or_target = prev;
      return 0;
    }
  } else if (cmp_ret == 0) {
    par_or_target = hint;
    return 0;
  }
  return RbFindFirst(src, par_or_target, header, value_cmp_pair);
}

RbNode *RbFindUpperBound(
    const void *src, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair) {
  auto *&root = header->par;
  if (!root) {
    return nullptr;
  }
  if (value_cmp_pair.func(value_cmp_pair.args, header->right, src) <= 0) {
    return nullptr;
  }
  auto *cur = root;
  RbNode *upper = nullptr;
  while (true) {
    int cmp_ret = (value_cmp_pair.func(value_cmp_pair.args, cur, src));
    if (cmp_ret > 0) {
      upper = cur;
      if (!cur->left) {
        break;
      }
      cur = cur->left;
    } else {
      if (!cur->right) {
        break;
      }
      cur = cur->right;
    }
  }
  return upper;
}

RbNode *RbFindLowerBound(
    const void *src, RbNode *header,
    const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair) {
  auto *&root = header->par;
  if (!root) {
    return nullptr;
  }
  if (value_cmp_pair.func(value_cmp_pair.args, header->right, src) < 0) {
    return nullptr;
  }
  auto *cur = root;
  RbNode *lower = nullptr;
  int cmp_ret;
  while (true) {
    cmp_ret = value_cmp_pair.func(value_cmp_pair.args, cur, src);
    if (cmp_ret < 0) {
      if (!cur->right) {
        break;
      }
      cur = cur->right;
    } else {
      lower = cur;
      if (!cur->left) {
        break;
      }
      cur = cur->left;
    }
  }
  return lower;
}

RbNode *RbAdd(const void *src, RbNode *header,
              const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
              const rb_pair::Pair<rb_pair::NodeAllocFunc> &node_alloc_pair) {
  auto *node = node_alloc_pair.func(node_alloc_pair.args, src);
  if (!node) {
    return nullptr;
  }
  auto *&root = header->par;
  do {
    if (!root) {
      RbAddNode(header->left, header, node, header);
      return node;
    }
    auto *&min = header->left;
    if (value_cmp_pair.func(value_cmp_pair.args, min, src) >= 0) {
      RbAddNode(min->left, min, node, header);
      return node;
    }
    auto *&max = header->right;
    if (value_cmp_pair.func(value_cmp_pair.args, max, src) <= 0) {
      RbAddNode(max->right, max, node, header);
      return node;
    }
    RbNode *par;
    RbAddNode(RbFind(src, par, header, value_cmp_pair), par, node, header);
  } while (false);
  return node;
}

RbNode *RbAdd(RbNode *hint, const void *src, RbNode *header,
              const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
              const rb_pair::Pair<rb_pair::NodeAllocFunc> &node_alloc_pair) {
  auto *node = node_alloc_pair.func(node_alloc_pair.args, src);
  if (node) {
    RbNode *par;
    RbAddNode(RbFindHint(src, par, hint, header, value_cmp_pair), par, node,
              header);
  }
  return node;
}

RbNode *
RbUniqueAdd(const void *src, RbNode *header,
            const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
            const rb_pair::Pair<rb_pair::NodeAllocFunc> &node_alloc_pair) {
  auto *&root = header->par;
  RbNode *node = nullptr;
  if (!root) {
    node = node_alloc_pair.func(node_alloc_pair.args, src);
    if (node) {
      RbAddNode(header->left, header, node, header);
    }
    return node;
  }
  auto *&min = header->left;
  int cmp_ret = value_cmp_pair.func(value_cmp_pair.args, min, src);
  if (cmp_ret > 0) {
    node = node_alloc_pair.func(node_alloc_pair.args, src);
    if (node) {
      RbAddNode(min->left, min, node, header);
    }
    return node;
  }
  if (cmp_ret == 0) {
    return nullptr;
  }
  auto *&max = header->right;
  if ((cmp_ret = value_cmp_pair.func(value_cmp_pair.args, max, src)) < 0) {
    node = node_alloc_pair.func(node_alloc_pair.args, src);
    if (node) {
      RbAddNode(max->right, max, node, header);
    }
    return node;
  }
  if (cmp_ret == 0) {
    return nullptr;
  }
  RbNode *par_or_target;
  auto ret = RbFindFirst(src, par_or_target, header, value_cmp_pair);
  if (ret != 0) {
    node = node_alloc_pair.func(node_alloc_pair.args, src);
    if (node) {
      auto *&child = ret == -1 ? par_or_target->left : par_or_target->right;
      RbAddNode(child, par_or_target, node, header);
    }
  }
  return node;
}

RbNode *
RbUniqueAdd(RbNode *hint, const void *src, RbNode *header,
            const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
            const rb_pair::Pair<rb_pair::NodeAllocFunc> &node_alloc_pair) {
  RbNode *par_or_target;
  auto ret = RbFindFirstHint(src, par_or_target, hint, header, value_cmp_pair);
  RbNode *node = nullptr;
  if (ret != 0) {
    node = node_alloc_pair.func(node_alloc_pair.args, src);
    if (node) {
      auto *&child = ret == -1 ? par_or_target->left : par_or_target->right;
      RbAddNode(child, par_or_target, node, header);
    }
  }
  return node;
}

bool RbRemove(const void *src, RbNode *header,
              const rb_pair::Pair<rb_pair::ValueCompareFunc> &value_cmp_pair,
              const rb_pair::Pair<rb_pair::NodeDropFunc> &node_drop_pair) {
  RbNode *target = nullptr;
  do {
    if (!header->par) {
      break;
    }
    int cmp_ret;
    if ((cmp_ret =
             value_cmp_pair.func(value_cmp_pair.args, header->left, src)) > 0) {
      return false;
    }
    if (cmp_ret == 0) {
      target = header->left;
      break;
    }
    if ((cmp_ret = value_cmp_pair.func(value_cmp_pair.args, header->right,
                                       src)) < 0) {
      return false;
    }
    if (cmp_ret == 0) {
      target = header->right;
      break;
    }
    int find_first_ret = RbFindFirst(src, target, header, value_cmp_pair);
    if (find_first_ret != 0) {
      target = nullptr;
    }
  } while (false);
  if (target) {
    auto *del_node = RbRemoveNode(target, header);
    node_drop_pair.func(node_drop_pair.args, del_node);
    return true;
  } else {
    return false;
  }
}

void RbClear(RbNode *node,
             const rb_pair::Pair<rb_pair::NodeDropFunc> &node_drop_pair) {
  if (node) {
    if (node->left) {
      RbClear(node->left, node_drop_pair);
    }
    if (node->right) {
      RbClear(node->right, node_drop_pair);
    }
    node_drop_pair.func(node_drop_pair.args, node);
  }
}

RbNode *RbCopy(RbNode *top, RbNode *par,
               const rb_pair::Pair<rb_pair::NodeCloneFunc> &node_clone_pair,
               const rb_pair::Pair<rb_pair::NodeDropFunc> &node_drop_pair) {
  RbNode *node = nullptr;
  if (top) {
    node = node_clone_pair.func(node_clone_pair.args, top);
    bool success = true;
    if (top->left) {
      success = (node->left =
                     RbCopy(top->left, node, node_clone_pair, node_drop_pair));
    }
    if (success && top->right) {
      success = (node->right =
                     RbCopy(top->right, node, node_clone_pair, node_drop_pair));
    }
    if (success) {
      node->par = par;
    } else {
      if (node->left) {
        RbClear(node->left, node_drop_pair);
      }
      node_drop_pair.func(node_drop_pair.args, node);
      node = nullptr;
    }
  }
  return node;
}
} // namespace xcl