// delete.c

#include <stdlib.h>

#include "rbtree.h"

static void delete_subtree(rbtree_t *tree, rbtree_node_t *node) {
    if (node->left != &tree->nil_node) {
        delete_subtree(tree, node->left);
    }
    if (node->right != &tree->nil_node) {
        delete_subtree(tree, node->right);
    }
    if (tree->del_func != NULL) {
        tree->del_func(node);
    }
    if (node->parent != &tree->nil_node) {
        if (node->parent->left == node) {
            node->parent->left = &tree->nil_node;
        } else if (node->parent->right == node) {
            node->parent->right = &tree->nil_node;
        }
    }
    free(node);
    tree->node_count--;
}

void rbtree_delete(rbtree_t *tree, rbtree_node_t *subtree) {
    if (tree == NULL || tree->root == &tree->nil_node) {
        return;
    }
    if (subtree == NULL || subtree == &tree->nil_node) {
        subtree = tree->root;
    }
    delete_subtree(tree, subtree);
}