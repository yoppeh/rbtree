// new.c

#include <stdlib.h>

#include "rbtree.h"

rbtree_t *rbtree_new(rbtree_key_compare_func_t cmp_func, rbtree_node_delete_func_t del_func) {
    rbtree_t *rbtree = calloc(1, sizeof(rbtree_t));
    if (rbtree != NULL) {
        rbtree->root = &rbtree->nil_node;
        rbtree->cmp_func = cmp_func;
        rbtree->del_func = del_func;
        rbtree->nil_node.parent = 
            rbtree->nil_node.left = 
            rbtree->nil_node.right = 
            &rbtree->nil_node;
        rbtree->nil_node.flags = RBTREE_COLOR_BLACK;
    }
    return(rbtree);
}