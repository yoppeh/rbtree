/**
 * @file rbtree.c
 * @author Warren Mann (warren@nonvol.io)
 * @brief rbtree implementation
 * @version 0.1
 * @date 2024-01-24
 * 
 * @copyright Copyright (c) 2024, Warren Mann
 */

#include <stdlib.h>

#include "rbtree.h"

static void delete_fixup(rbtree_t *tree, rbtree_node_t *x);
static void delete_subtree(rbtree_t *tree, rbtree_node_t *node);
static void insert_fixup(rbtree_t *tree, rbtree_node_t *node);
static void rotate_left(rbtree_t *tree, rbtree_node_t *x);
static void rotate_right(rbtree_t *tree, rbtree_node_t *x);
static void transplant(rbtree_t *tree, rbtree_node_t *u, rbtree_node_t *v);

void rbtree_delete(rbtree_t *tree, rbtree_node_t *subtree) {
    if (tree == NULL || tree->root == &tree->nil_node) {
        return;
    }
    if (subtree == NULL || subtree == &tree->nil_node) {
        subtree = tree->root;
    }
    delete_subtree(tree, subtree);
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
        delete_fixup(tree, x);
    }
    tree->node_count--;
}

void rbtree_free(rbtree_t *tree) {
    if (tree != NULL) {
        if (tree->root != &tree->nil_node) {
            rbtree_delete(tree, NULL);
        }
    }
    free(tree);
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
            return child;
        } else {
            child = child->right;
        }
    }
    node = malloc(sizeof(rbtree_node_t));
    if (node == NULL) {
        return NULL;
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
    insert_fixup(tree, node);
    tree->node_count++;
    return node;
}

rbtree_node_t *rbtree_lookup(rbtree_t *tree, void *key) {
    if (tree == NULL) {
        return NULL;
    }
    rbtree_node_t *node = tree->root;
    while (node != &tree->nil_node) {
        int i = tree->cmp_func(key, node->key);
        if (i == 0) {
            return node;
        } else if (i < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return NULL;
}

rbtree_node_t *rbtree_maximum(rbtree_t *tree, rbtree_node_t *subtree) {
    if (subtree == NULL) {
        subtree = tree->root;
    }
    while (subtree != &tree->nil_node && subtree->right != &tree->nil_node) {
        subtree = subtree->right;
    }
    return subtree;
}

rbtree_node_t *rbtree_minimum(rbtree_t *tree, rbtree_node_t *subtree) {
    if (subtree == NULL) {
        subtree = tree->root;
    }
    while (subtree != &tree->nil_node && subtree->left != &tree->nil_node) {
        subtree = subtree->left;
    }
    return subtree;
}

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
    return rbtree;
}

int rbtree_traverse_ascending(rbtree_t *tree, rbtree_node_t *subtree, rbtree_traverse_func_t cb) {
    int i;
    if (tree == NULL || tree->root == &tree->nil_node) {
        return -1;
    }
    if (subtree == NULL || subtree == &tree->nil_node) {
        subtree = tree->root;
    }
    if (subtree->left != &tree->nil_node) {
        i = rbtree_traverse_ascending(tree, subtree->left, cb);
        if (i != 0) {
            return i;
        }
    }
    if (cb != NULL) {
        i = cb(subtree);
        if (i != 0) {
            return i;
        }
    }
    if (subtree->right != &tree->nil_node) {
        i = rbtree_traverse_ascending(tree, subtree->right, cb);
        if (i != 0) {
            return i;
        }
    }
    return 0;
}

int rbtree_traverse_descending(rbtree_t *tree, rbtree_node_t *subtree, rbtree_traverse_func_t cb) {
    int i;
    if (tree == NULL || tree->root == &tree->nil_node) {
        return -1;
    }
    if (subtree == NULL || subtree == &tree->nil_node) {
        subtree = tree->root;
    }
    if (subtree->right != &tree->nil_node) {
        i = rbtree_traverse_descending(tree, subtree->right, cb);
        if (i != 0) {
            return i;
        }
    }
    if (cb != NULL) {
        int i = cb(subtree);
        if (i != 0) {
            return i;
        }
    }
    if (subtree->left != &tree->nil_node) {
        i = rbtree_traverse_descending(tree, subtree->left, cb);
        if (i != 0) {
            return i;
        }
    }
    return 0;
}

static void delete_fixup(rbtree_t *tree, rbtree_node_t *x) {
    rbtree_node_t *w;
    while (x != tree->root && RBTREE_COLOR_IS_BLACK(x)) {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (RBTREE_COLOR_IS_RED(w)) {
                RBTREE_SET_BLACK(w);
                RBTREE_SET_RED(x->parent);
                rotate_left(tree, x->parent);
                w = x->parent->right;
            }
            if (RBTREE_COLOR_IS_BLACK(w->left) && RBTREE_COLOR_IS_BLACK(w->right)) {
                RBTREE_SET_RED(w);
                x = x->parent;
            } else {
                if (RBTREE_COLOR_IS_BLACK(w->right)) {
                    RBTREE_SET_RED(w);
                    RBTREE_SET_BLACK(w->left);
                    rotate_right(tree, w);
                    w = x->parent->right;
                }
                w->flags &= RBTREE_USER_MASK;
                w->flags |= x->parent->flags & RBTREE_COLOR_MASK;
                RBTREE_SET_BLACK(x->parent);
                RBTREE_SET_BLACK(w->right);
                rotate_left(tree, x->parent);
                x = tree->root;
            }
        } else {
            w = x->parent->left;
            if (RBTREE_COLOR_IS_RED(w)) {
                RBTREE_SET_BLACK(w);
                RBTREE_SET_RED(x->parent);
                rotate_right(tree, x->parent);
                w = x->parent->left;
            }
            if (RBTREE_COLOR_IS_BLACK(w->left) && RBTREE_COLOR_IS_BLACK(w->right)) {
                RBTREE_SET_RED(w);
                x = x->parent;
            } else {
                if (RBTREE_COLOR_IS_BLACK(w->left)) {
                    RBTREE_SET_RED(w);
                    RBTREE_SET_BLACK(w->right);
                    rotate_left(tree, w);
                    w = x->parent->left;
                }
                w->flags &= RBTREE_USER_MASK;
                w->flags |= x->parent->flags & RBTREE_COLOR_MASK;
                RBTREE_SET_BLACK(x->parent);
                RBTREE_SET_BLACK(w->left);
                rotate_right(tree, x->parent);
                x = tree->root;
            }
        }
    }
    RBTREE_SET_BLACK(x);
}

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

static void insert_fixup(rbtree_t *tree, rbtree_node_t *node) {
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
                    rotate_left(tree, node);
                }
                RBTREE_SET_BLACK(node->parent);
                RBTREE_SET_RED(node->parent->parent);
                rotate_right(tree, node->parent->parent);
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
                    rotate_right(tree, node);
                }
                RBTREE_SET_BLACK(node->parent);
                RBTREE_SET_RED(node->parent->parent);
                rotate_left(tree, node->parent->parent);
            }
        }
    }
    RBTREE_SET_BLACK(tree->root);
    tree->nil_node.left = tree->nil_node.right = tree->nil_node.parent = &tree->nil_node;
    tree->nil_node.flags = RBTREE_COLOR_BLACK;
}

static void rotate_left(rbtree_t *tree, rbtree_node_t *x) {
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

static void rotate_right(rbtree_t *tree, rbtree_node_t *x) {
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
