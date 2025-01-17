#include <internal/util/rb.h>
#include <stddef.h>
#include <string.h>

struct RbNode* Rb_parent(struct RbNode* node) {
  return (struct RbNode*)(node->compressed_par << 1);
}
void Rb_setPar(struct RbNode* node, struct RbNode* parent) {
  node->compressed_par = ((uintptr_t)parent >> 1);
}
static void Rb_swapColor(struct RbNode* node1, struct RbNode* node2) {
  RbColor color = node1->color;
  node1->color = node2->color;
  node2->color = color;
}

static void Rb_leftRotate(struct RbNode* node, struct RbNode* header) {
  struct RbNode *r, *rl;
  r = node->right;
  rl = r->left;
  node->right = rl;
  if (rl)
    Rb_setPar(rl, node);
  struct RbNode* par = Rb_parent(node);
  if (par == header)
    Rb_setPar(header, r);
  else if (node == par->left)
    par->left = r;
  else
    par->right = r;
  Rb_setPar(r, par);
  r->left = node;
  Rb_setPar(node, r);
}

static void Rb_rightRotate(struct RbNode* node, struct RbNode* header) {
  struct RbNode *l, *lr;
  l = node->left;
  lr = l->right;
  node->left = lr;
  if (lr)
    Rb_setPar(lr, node);
  struct RbNode* par = Rb_parent(node);
  if (par == header)
    Rb_setPar(header, l);
  else if (par->left == node)
    par->left = l;
  else
    par->right = l;
  Rb_setPar(l, par);
  l->right = node;
  Rb_setPar(node, l);
}

static void Rb_addFixup(struct RbNode* node, struct RbNode* header) {
  node->color = RED;
  struct RbNode* par;
  while ((par = Rb_parent(node))->color == RED && par != header) {
    struct RbNode* grand = Rb_parent(par);
    bool is_left = par->left == node;
    struct RbNode* uncle = par == grand->left ? grand->right : grand->left;
    if (uncle && uncle->color == RED) {
      uncle->color = BLACK;
      par->color = BLACK;
      grand->color = RED;
      node = grand;
      continue;
    }
    if (is_left != (par == grand->left)) {
      is_left ? Rb_rightRotate(par, header) : Rb_leftRotate(par, header);
      node = par;
      par = Rb_parent(node);
    }
    par == grand->left ? Rb_rightRotate(grand, header) : Rb_leftRotate(grand, header);
    par->color = BLACK;
    grand->color = RED;
    break;
  }
  Rb_parent(header)->color = BLACK;
}

static void Rb_removeFixup(struct RbNode* node, struct RbNode* par, struct RbNode* header) {
  while ((NULL == node || node->color == BLACK) && par != header) {
    bool is_left_node = node == par->left;
    struct RbNode* sibling = is_left_node ? par->right : par->left;
    if (sibling->color == RED) {
      sibling->color = BLACK;
      par->color = RED;
      is_left_node ? Rb_leftRotate(par, header) : Rb_rightRotate(par, header);
      sibling = is_left_node ? par->right : par->left;
    }
    if ((NULL == sibling->left || sibling->left->color == BLACK) &&
        (NULL == sibling->right || sibling->right->color == BLACK)) {
      sibling->color = RED;
      node = par;
      par = Rb_parent(node);
      continue;
    }
    if (is_left_node && (NULL == sibling->right || sibling->right->color == BLACK)) {
      sibling->left->color = BLACK;
      sibling->color = RED;
      Rb_rightRotate(sibling, header);
      sibling = par->right;
    } else if ((NULL == sibling->left || sibling->left->color == BLACK) && !is_left_node) {
      sibling->right->color = BLACK;
      sibling->color = RED;
      Rb_leftRotate(sibling, header);
      sibling = par->left;
    }
    sibling->color = par->color;
    par->color = BLACK;
    if (is_left_node) {
      sibling->right->color = BLACK;
      Rb_leftRotate(par, header);
    } else {
      sibling->left->color = BLACK;
      Rb_rightRotate(par, header);
    }
    break;
  }
  if (node)
    node->color = BLACK;
}

void Rb_init(struct RbNode* header) {
  header->compressed_par = 0;
  header->color = RED;
  header->left = header->right = NULL;
}

struct RbNode* Rb_root(struct RbNode* header) { return Rb_parent(header); }

struct RbNode* Rb_rightMost(struct RbNode* node) {
  while (node->right)
    node = node->right;
  return node;
}

struct RbNode* Rb_leftMost(struct RbNode* node) {
  while (node->left)
    node = node->left;
  return node;
}

struct RbNode* Rb_max(struct RbNode* header) { return header->left; }

struct RbNode* Rb_min(struct RbNode* header) { return header->right; }

void Rb_add(bool left, struct RbNode* node, struct RbNode* par, struct RbNode* header) {
  node->left = node->right = NULL;
  Rb_setPar(node, par);
  if (par == header) {
    Rb_setPar(header, node);
    header->left = node;
    header->right = node;
    node->color = BLACK;
    return;
  }
  if (left) {
    par->left = node;
    if (par == header->left)
      header->left = node;
  } else {
    par->right = node;
    if (par == header->right)
      header->right = node;
  }
  Rb_addFixup(node, header);
}

struct RbNode* Rb_remove(struct RbNode* node, struct RbNode* header) {
  struct RbNode *x, *y, *z;
  x = node;
  y = NULL == node->left    ? node->right
      : NULL == node->right ? node->left
                            : (x = Rb_rightMost(node->left))->left;
  struct RbNode* par = Rb_parent(node);
  if (x != node) {
    x->right = node->right;
    Rb_setPar(node->right, x);
    if (x == node->left) {
      z = x;
    } else {
      x->left = node->left;
      Rb_setPar(node->left, x);
      z = Rb_parent(x);
      z->right = y;
      if (y)
        Rb_setPar(y, z);
    }
    if (par == header)
      Rb_setPar(header, x);
    else if (par->left == node)
      par->left = x;
    else
      par->right = x;
    Rb_setPar(x, par);
    Rb_swapColor(node, x);
  } else {
    z = par;
    if (y)
      Rb_setPar(y, z);
    if (par == header)
      Rb_setPar(header, y);
    else if (par->left == node)
      par->left = y;
    else
      par->right = y;
    if (node == header->left)
      header->left = node->right ? Rb_leftMost(y) : z;
    if (node == header->right)
      header->right = node->left ? Rb_rightMost(y) : z;
  }
  if (node->color == BLACK)
    Rb_removeFixup(y, z, header);
  return node;
}

struct RbNode* Rb_next(struct RbNode* node, struct RbNode* header) {
  if (node == header || node == header->right)
    return header;
  if (node->right)
    return Rb_leftMost(node->right);
  struct RbNode* par;
  while ((par = Rb_parent(node)) != header) {
    if (par->left == node)
      break;
    node = par;
  }
  return par;
}

struct RbNode* Rb_prev(struct RbNode* node, struct RbNode* header) {
  if (node == header)
    return node->right;
  if (node == header->left)
    return node;
  if (node->left)
    return Rb_rightMost(node->left);
  struct RbNode* par;
  for (;;) {
    par = Rb_parent(node);
    if (par->right == node)
      return par;
    node = par;
  }
}

bool Rb_empty(struct RbNode* header) { return Rb_parent(header) == NULL; }
