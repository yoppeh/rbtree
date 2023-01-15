// deletenode.c

#include <stdlib.h>

#include "rbtree.h"

extern void rbtree_rotate_right(rbtree_t *tree, rbtree_node_t *x);
extern void rbtree_rotate_left(rbtree_t *tree, rbtree_node_t *x);

void rbtree_delete_fixup(rbtree_t *tree, rbtree_node_t *x) {
    rbtree_node_t *w;
    while (x != tree->root && RBTREE_COLOR_IS_BLACK(x)) {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (RBTREE_COLOR_IS_RED(w)) {
                RBTREE_SET_BLACK(w);
                RBTREE_SET_RED(x->parent);
                rbtree_rotate_left(tree, x->parent);
                w = x->parent->right;
            }
            if (RBTREE_COLOR_IS_BLACK(w->left) && RBTREE_COLOR_IS_BLACK(w->right)) {
                RBTREE_SET_RED(w);
                x = x->parent;
            } else {
                if (RBTREE_COLOR_IS_BLACK(w->right)) {
                    RBTREE_SET_RED(w);
                    RBTREE_SET_BLACK(w->left);
                    rbtree_rotate_right(tree, w);
                    w = x->parent->right;
                }
                w->flags &= RBTREE_USER_MASK;
                w->flags |= x->parent->flags & RBTREE_COLOR_MASK;
                RBTREE_SET_BLACK(x->parent);
                RBTREE_SET_BLACK(w->right);
                rbtree_rotate_left(tree, x->parent);
                x = tree->root;
            }
        } else {
            w = x->parent->left;
            if (RBTREE_COLOR_IS_RED(w)) {
                RBTREE_SET_BLACK(w);
                RBTREE_SET_RED(x->parent);
                rbtree_rotate_right(tree, x->parent);
                w = x->parent->left;
            }
            if (RBTREE_COLOR_IS_BLACK(w->left) && RBTREE_COLOR_IS_BLACK(w->right)) {
                RBTREE_SET_RED(w);
                x = x->parent;
            } else {
                if (RBTREE_COLOR_IS_BLACK(w->left)) {
                    RBTREE_SET_RED(w);
                    RBTREE_SET_BLACK(w->right);
                    rbtree_rotate_left(tree, w);
                    w = x->parent->left;
                }
                w->flags &= RBTREE_USER_MASK;
                w->flags |= x->parent->flags & RBTREE_COLOR_MASK;
                RBTREE_SET_BLACK(x->parent);
                RBTREE_SET_BLACK(w->left);
                rbtree_rotate_right(tree, x->parent);
                x = tree->root;
            }
        }
    }
    RBTREE_SET_BLACK(x);
}

static void transplant(rbtree_t *tree, rbtree_node_t *u, rbtree_node_t *v) {
    if (u->parent == &tree->nil_node) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

void rbtree_delete_node(rbtree_t *tree, rbtree_node_t *node) {
    rbtree_node_t *w = node;
    rbtree_node_t *x;
    rbtree_node_t *y = node;
    uint32_t color = y->flags & RBTREE_COLOR_MASK;
    if (node->left == &tree->nil_node) {
        x = node->right;
        transplant(tree, node, node->right);
    } else if (node->right == &tree->nil_node) {
        x = node->left;
        transplant(tree, node, node->left);
    } else {
        y = rbtree_minimum(tree, node->right);
        color = y->flags & RBTREE_COLOR_MASK;
        x = y->right;
        if (y->parent == node) {
            x->parent = y;
        } else {
            transplant(tree, y, y->right);
            y->right = node->right;
            y->right->parent = y;
        }
        transplant(tree, node, y);
        y->left = node->left;
        y->left->parent = y;
        y->flags &= RBTREE_USER_MASK;
        y->flags |= node->flags & RBTREE_COLOR_MASK;
    }
    if (tree->del_func != NULL) {
        tree->del_func(w);
    }
    free(w);
    if (color == 0) {
        rbtree_delete_fixup(tree, x);
    }
    tree->node_count--;
}