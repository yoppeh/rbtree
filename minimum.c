// minimum.c

#include <stdlib.h>

#include "rbtree.h"

rbtree_node_t *rbtree_minimum(rbtree_t *tree, rbtree_node_t *subtree) {
    if (subtree == NULL) {
        subtree = tree->root;
    }
    while (subtree != &tree->nil_node && subtree->left != &tree->nil_node) {
        subtree = subtree->left;
    }
    return(subtree);
}
