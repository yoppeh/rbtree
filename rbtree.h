// rbtree.h

#ifndef RBTREE_H
#define RBTREE_H

#include <stdint.h>

// Flags for tracking the color of a node. The high-order bit of the flags 
// field is used for color tracking. The remaining bits can be used by the
// application for whatever it wants and won't be modified by the rbtree
// code. The user data can be accessed with RBTREE_SET_USER(n, d), 
// RBTREE_GET_USER(n) and the RBTREE_USER_MASK bit mask.
#define RBTREE_COLOR_MASK           0x80000000
#define RBTREE_USER_MASK            0x7fffffff
#define RBTREE_COLOR_BLACK          0x00000000
#define RBTREE_COLOR_RED            0x80000000
#define RBTREE_COLOR(n)             ((n)->flags & RBTREE_COLOR_MASK)
#define RBTREE_COLOR_IS_BLACK(n)    (RBTREE_COLOR(n) == RBTREE_COLOR_BLACK)
#define RBTREE_COLOR_IS_RED(n)      (RBTREE_COLOR(n) == RBTREE_COLOR_RED)
#define RBTREE_SET_BLACK(n)         (n)->flags &= RBTREE_USER_MASK
#define RBTREE_SET_RED(n)           (n)->flags |= RBTREE_COLOR_RED
#define RBTREE_SET_USER(n, d)       (n)->flags = ((n)->flags & RBTREE_COLOR_MASK) | ((uint32_t)d & RBTREE_USER_MASK)
#define RBTREE_GET_USER(n)          ((n)->flags & RBTREE_USER_MASK)

// A red-black tree node structure. The data field is not referenced by the 
// rbtree code in any way. The parent, left and right fields should not be
// altered by the application. The key field is set and used by the rbtree
// code internally, but it is the application's responsibility to free the
// key field in the node_delete_func_t that is called just prior to a node
// being deleted (if necessary).
typedef struct rbtree_node_t {
    // This node's parent in the red-black tree.
    struct rbtree_node_t *parent;
    // Subtree with keys having a lower ordinal value than this node.
    struct rbtree_node_t *left;
    // Subtree with keys having a higher ordinal value than this node.
    struct rbtree_node_t *right;
    // Highest-order bit is used for red-black tracking, other bits are
    // are zeroed upon node creation, then never again altered by the
    // rbtree code and may be used by the application if desired.
    uint32_t flags;
    // The key value used to order the nodes.
    void *key;
    // User data.
    void *data;
} rbtree_node_t;

// The key_compare_func_t is a function used to compare rbtree_node_t node 
// keys. This function is required for properly ordering the nodes.
//
// It should return: 
//  -1 if a < b
//   0 if a == b
//   1 if a > b
typedef int (*rbtree_key_compare_func_t)(void *a, void *b);

// The node_delete_func_t is a function that will receive a node just before it
// is deleted. This gives the owner the opportunity to free the key and data, 
// if necessary. If no memory or other cleanup needs to be done upon node 
// delete, this can be NULL.
typedef void (*rbtree_node_delete_func_t)(rbtree_node_t *node);

// Function to call for each node visited during a traversal. This function can
// return a value that is non-zero to stop the traversal early.
typedef int (*rbtree_traverse_func_t)(rbtree_node_t *node);

// A red-black tree. This structure tracks the tree root (which will change as
// nodes are added), the key comparison and node delete functions and keeps a
// special nil node that is used internally for tree maintenance.
typedef struct rbtree_t {
    // The root of the red-black tree.
    rbtree_node_t *root;
    // Callback function for comparing key values.
    rbtree_key_compare_func_t cmp_func;
    // Callback function allowing application to delete its key and data if
    // needed. This can be NULL if user data doesn't need to be freed.
    rbtree_node_delete_func_t del_func;
    // NIL node used by the red-black tree code.
    rbtree_node_t nil_node;
    // The number of nodes in the tree.
    unsigned long int node_count;
} rbtree_t;

// Delete the entire sub-tree structure rooted at subtree. If subtree is NULL, 
// the entire red-black tree is deallocated and the rbtree_t structure itself 
// is deallocated. The specified del_func is called for each node prior to its
// deletion (assuming a non-NULL del_func was given).
void rbtree_delete(rbtree_t *t, rbtree_node_t *node);

// Delete a node from the tree. The del_func, if not NULL, will be called with
// the given node as the argument before the node itself is deleted.
void rbtree_delete_node(rbtree_t *tree, rbtree_node_t *node);

// Delete all nodes in the tree and frees memory allocated to the tree
// structure.
void rbtree_free(rbtree_t *tree);

// Insert a new node with the given key into the tree. The cmp_func will be
// called to compare the given key with the keys of other nodes in order to
// determine where the node should be inserted. If a node with this key is
// already present in the tree, no new node is created and the pointer to
// THAT node is returned.
rbtree_node_t *rbtree_insert(rbtree_t *tree, void *key);

// Look up a node with the given key. If the node with a matching key is 
// found, a pointer to it is returned to the caller. The cmp_func will be
// used to find the key in the tree. If the key is not found in the tree,
// NULL is returned.
rbtree_node_t *rbtree_lookup(rbtree_t *tree, void *key);

// Return the node with the key value with the highest ordinality rooted in
// the specified subtree. If the tree is empty, NULL is returned. If subtree is
// NULL, the root of the red-black tree is used as the starting  subtree.
rbtree_node_t *rbtree_maximum(rbtree_t *tree, rbtree_node_t *subtree);

// Return the node with the key value with the lowest ordinality rooted in
// the specified subtree. If the tree is empty, NULL is returned. If subtree is
// NULL, the root of the red-black tree is used as the starting subtree.
rbtree_node_t *rbtree_minimum(rbtree_t *tree, rbtree_node_t *subtree);

// Create a new red-black tree. The cmp_func is a required function that 
// compares two key values and returns -1, if a is less than b, 0 if the two 
// compared keys are equal, and +1 if a is greater than b. The del_func is an 
// optional function that is called with a node just before it is deleted. This
// gives the caller an opportunity to delete memory allocated for the key 
// and/or data belonging to the node.
rbtree_t *rbtree_new(rbtree_key_compare_func_t cmp_func, rbtree_node_delete_func_t del_func);

// Traverse a subtree in order from lowest ordinal key to highest ordinal key.
// If the subtree is NULL, then the traversal is across the entire tree. The 
// specified callback function is called for every node visited, unless it is 
// NULL, in which case this function is less than useful. If cb returns a 
// non-zero value, the traversal will abort and return that value to the caller.
// If the traversal ends normally at the last node, 0 is returned. Otherwise, -1
// will be returned.
int rbtree_traverse_ascending(rbtree_t *tree, rbtree_node_t *subtree, rbtree_traverse_func_t cb);

// Traverse a subtree in order from highest ordinal key to lowest ordinal key.
// If the subtree is NULL, then the traversal is across the entire tree. The 
// specified callback function is called for every node  visited, unless it is 
// NULL, in which case this function is less than useful. If cb returns a 
// non-zero value, the traversal will abort and return that value to the caller.
// If the traversal ends normally at the last node, 0 is returned. Otherwise, -1
// will be returned.
int rbtree_traverse_descending(rbtree_t *tree, rbtree_node_t *subtree, rbtree_traverse_func_t cb);

#endif /* RBTREE_H */