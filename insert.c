// insert.c 

#include <stdio.h>
#include <stdlib.h>

#include "rbtree.h"

extern void rbtree_rotate_right(rbtree_t *tree, rbtree_node_t *x);
extern void rbtree_rotate_left(rbtree_t *tree, rbtree_node_t *x);

static void rbtree_insert_fixup(rbtree_t *tree, rbtree_node_t *node) {
    while (RBTREE_COLOR_IS_RED(node->parent)) {
        rbtree_node_t *parent = node->parent;
        rbtree_node_t *grandparent = parent->parent;
        if (parent == grandparent->left) {
            if (RBTREE_COLOR_IS_RED(grandparent->right)) {
                RBTREE_SET_BLACK(parent);
                RBTREE_SET_BLACK(grandparent->right);
                RBTREE_SET_RED(grandparent);
                node = grandparent;
            } else {
                if (node == parent->right) {
                    node = parent;
                    rbtree_rotate_left(tree, node);
                }
                RBTREE_SET_BLACK(node->parent);
                RBTREE_SET_RED(node->parent->parent);
                rbtree_rotate_right(tree, node->parent->parent);
            }
        } else {
            if (RBTREE_COLOR_IS_RED(grandparent->left)) {
                RBTREE_SET_BLACK(parent);
                RBTREE_SET_BLACK(grandparent->left);
                RBTREE_SET_RED(grandparent);
                node = grandparent;
            } else {
                if (node == parent->left) {
                    node = parent;
                    rbtree_rotate_right(tree, node);
                }
                RBTREE_SET_BLACK(node->parent);
                RBTREE_SET_RED(node->parent->parent);
                rbtree_rotate_left(tree, node->parent->parent);
            }
        }
    }
    RBTREE_SET_BLACK(tree->root);
    tree->nil_node.left = tree->nil_node.right = tree->nil_node.parent = &tree->nil_node;
    tree->nil_node.flags = RBTREE_COLOR_BLACK;
}

rbtree_node_t *rbtree_insert(rbtree_t *tree, void *key) {
    rbtree_node_t *child = tree->root;
    rbtree_node_t *parent = &tree->nil_node;
    rbtree_node_t *node = NULL;
    uint32_t color = RBTREE_COLOR_RED;
    int i = 0;
    while (child != &tree->nil_node) {
        parent = child;
        i = tree->cmp_func(key, child->key);
        if (i < 0) {
            child = child->left;
        } else if (i == 0) {
            return(child);
        } else {
            child = child->right;
        }
    }
    node = malloc(sizeof(rbtree_node_t));
    if (node == NULL) {
        return(NULL);
    }
    if (parent == &tree->nil_node) {
        tree->root = node;
        color = RBTREE_COLOR_BLACK;
    } else if (i < 0) {
        parent->left = node;
    } else {
        parent->right = node;
    }
    node->parent = parent;
    node->left = &tree->nil_node;
    node->right = &tree->nil_node;
    node->flags = color;
    node->key = key;
    rbtree_insert_fixup(tree, node);
    tree->node_count++;
    return(node);
}