// lookup.c

#include <stdlib.h>

#include "rbtree.h"

rbtree_node_t *rbtree_lookup(rbtree_t *tree, void *key) {
    if (tree == NULL) {
        return(NULL);
    }
    rbtree_node_t *node = tree->root;
    while (node != &tree->nil_node) {
        int i = tree->cmp_func(key, node->key);
        if (i == 0) {
            return(node);
        } else if (i < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return(NULL);
}