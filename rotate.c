// rotate.c

#include <stdlib.h>

#include "rbtree.h"

void rbtree_rotate_right(rbtree_t *tree, rbtree_node_t *x) {
    rbtree_node_t *y = x->left;
    x->left = y->right;
    if (y->right != &tree->nil_node) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (tree->root == x) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

void rbtree_rotate_left(rbtree_t *tree, rbtree_node_t *x) {
    rbtree_node_t *y = x->right;
    x->right = y->left;
    if (y->left != &tree->nil_node) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (tree->root == x) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}
