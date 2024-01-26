#include "avl.h"

static uint32_t avl_depth(AVLNode *node) {
  return node ? node->depth : 0;
}

static uint32_t avl_cnt(AVLNode *node) {
  return node ? node->cnt : 0;
}

static uint32_t max(uint32_t lhs, uint32_t rhs) {
  return lhs < rhs ? rhs : lhs;
}

// maintain the depth and cnt field
static void avl_update(AVLNode *node) {
  node->depth = 1 + max(avl_depth(node->left), avl_depth(node->right));
  node->cnt = 1 + avl_cnt(node->left) + avl_cnt(node->right);
}

/*   2             4
 *  / \           / \
 * 1   4   ==>   2   5
 *    / \       / \
 *   3   5     1   3
 */
static AVLNode *rot_left(AVLNode *node) {
  AVLNode *new_node = node->right;
  if (new_node->left) {
    new_node->left->parent = node;
  }
  node->right = new_node->left;    // rotation
  new_node->left = node;           // rotation
  new_node->parent = node->parent;
  node->parent = new_node;
  avl_update(node);
  avl_update(new_node);
  return new_node;
}

 static AVLNode *rot_right(AVLNode *node) {
  AVLNode *new_node = node->left;
  if (new_node->right) {
    new_node->right->parent = node;
  }
  node->left = new_node->right;    // rotation
  new_node->right = node;           // rotation
  new_node->parent = node->parent;
  node->parent = new_node;
  avl_update(node);
  avl_update(new_node);
  return new_node;
 }

// the left subtree is too deep
static AVLNode *avl_fix_left(AVLNode *root) {
  if (avl_depth(root->left->left) < avl_depth(root->left->right)) {
    root->left = rot_left(root->left); // a left rotation makes the left subtree
                                       // deeper than the right subtree
                                       // while keeping the rotated height if
                                       // the right subtree is deeper by 1
  }
  return rot_right(root); // a right rotation restores balance if
                          // the left subtree is deeper by 2, and the
                          // left left subtree is deeper than the left right subtree
}

// right subtree is too deep
static AVLNode *avl_fix_right(AVLNode *root) {
  if (avl_depth(root->right->right) < avl_depth(root->right->left)) {
    root->right = rot_right(root->right); // a right rotation makes the right subtree
                                          // deeper than the left subtree
                                          // while keeping the rotated height if
                                          // the left subtree is deeper by 1
  }
  return rot_left(root); // a left rotation restores balance if
                         // the right subtree is deeper by 2, and the
                         // right right subtree is deeper than the right left subtree
}

// fix imbalances nodes and maintain invariants until the root is reached
AVLNode *avl_fix(AVLNode *node) {
  while (true) {
    avl_update(node);
    uint32_t l = avl_depth(node->left);
    uint32_t r = avl_depth(node->right);
    AVLNode **from = NULL;
    if (node->parent) {
      from = (node->parent->left == node)
        ? &node->parent->left : &node->parent->right;
    }
    if (l == r + 2) {
      node = avl_fix_left(node);
    } else if (l + 2 == r) {
      node = avl_fix_right(node);
    }
    if (!from) {
      return node;
    }
    *from = node;
    node = node->parent;
  }
}

// detach a node and returns the new root of the tree
AVLNode *avl_del(AVLNode *node) {
  if (node->right == NULL) {
    // no right subtree, replace the node with the left subtree
    // link the left subtree to the parent
    AVLNode *parent = node->parent;
    if (node->left) {
      node->left->parent = parent;
    }
    if (parent) {
      // attach the left subtree to the parent
      (parent->left == node ? parent->left : parent->right) = node->left;
      return avl_fix(parent);
    } else {
      // removing root?
      return node->left;
    }
  } else {
    // detach the successor
    AVLNode *victim = node->right;
    while (victim->left) {
      victim = victim->left;
    }
    AVLNode *root = avl_del(victim);
    // swap with it
    *victim = *node;
    if (victim->left) {
      victim->left->parent = victim;
    }
    if (victim->right) {
      victim->right->parent = victim;
    }
    if (AVLNode *parent = node->parent) {
      (parent->left == node ? parent->left : parent->right) = victim;
      return root;
    } else {
      // removing root?
      return victim;
    }
  }
}

// offset into the succeeding or preceding node.
// note: the worst-case is O(log(n)) regardless of how long the offset is.
AVLNode *avl_offset(AVLNode *node, int64_t offset) {
  int64_t pos = 0; // relative to the starting node
  while (offset != pos) {
    if (pos < offset && pos + avl_cnt(node->right) >= offset) {
      // the target is inside the right subtree
      node = node->right;
      pos += avl_cnt(node->left) + 1;
    } else if (pos > offset && pos - avl_cnt(node->left) <= offset) {
      // the target is inside the left subtree
      node = node->left;
      pos -= avl_cnt(node->right) + 1;
    } else {
      // go to the parent
      AVLNode *parent = node->parent;
      if (!parent) {
        return NULL;
      }
      if (parent->right == node) {
        pos -= avl_cnt(node->left) + 1;
      } else {
        pos += avl_cnt(node->right) + 1;
      }
      node = parent;
    }
  }
  return node;
}
