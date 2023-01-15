// free.c

#include <stdlib.h>

#include "rbtree.h"

void rbtree_free(rbtree_t *tree) {
    if (tree != NULL) {
        if (tree->root != &tree->nil_node) {
            rbtree_delete(tree, NULL);
        }
    }
    free(tree);
}