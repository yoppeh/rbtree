// ascending.c

#include <stdlib.h>

#include "rbtree.h"

int rbtree_traverse_ascending(rbtree_t *tree, rbtree_node_t *subtree, rbtree_traverse_func_t cb) {
    int i;
    if (tree == NULL || tree->root == &tree->nil_node) {
        return(-1);
    }
    if (subtree == NULL || subtree == &tree->nil_node) {
        subtree = tree->root;
    }
    if (subtree->left != &tree->nil_node) {
        i = rbtree_traverse_ascending(tree, subtree->left, cb);
        if (i != 0) {
            return(i);
        }
    }
    if (cb != NULL) {
        i = cb(subtree);
        if (i != 0) {
            return(i);
        }
    }
    if (subtree->right != &tree->nil_node) {
        i = rbtree_traverse_ascending(tree, subtree->right, cb);
        if (i != 0) {
            return(i);
        }
    }
    return(0);
}
