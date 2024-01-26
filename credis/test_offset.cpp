#include <assert.h>

#include "common.h"
#include "avl.cpp"

struct Data {
  AVLNode node;
  uint32_t val = 0;
};

struct Container {
  AVLNode *root = NULL;
};

static void add(Container &c, uint32_t val) {
  Data *data = new Data();
  avl_init(&data->node);
  data->val = val;

  if (!c.root) {
    c.root = &data->node;
    return;
  }

  AVLNode *cur = c.root;
  while (true) {
    AVLNode **from =
      (val < container_of(cur, Data, node)->val)
      ? &cur->left : &cur->right;
    if (!*from) {
      *from = &data->node;
      data->node.parent = cur;
      c.root = avl_fix(&data->node);
      break;
    }
    cur = *from;
  }
}

static void dispose(AVLNode *node) {
  if (node) {
    dispose(node->left);
    dispose(node->right);
    delete container_of(node, Data, node);
  }
}

static void test_case(uint32_t sz) {
  Container c;
  for (uint32_t i = 0; i < sz; ++i) {
    add(c, i);
  }

  AVLNode *min = c.root;
  while(min->left) {
    min = min->left;
  }
  for (uint32_t i = 0; i < sz; ++i) {
  }

  dispose(c.root);
}

int main() {
  for (uint32_t i = 1; i < 500; ++i) {
    test_case(i);
  }
}
