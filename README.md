# rbtree

A red-black tree implementation in c.

The library has an `rbtree_t` type that describes the red-black tree as a whole. This structure is created with the `rbtree_new()` function. Once a tree is created with `rbtree_new()`, nodes can be added using the `rbtree_insert()` function. The tree can be searched with `rbtree_lookup()`. The minimum and maximum keys can be found with `rbtree_minimum()` and `rbtree_maximum()`. A node can be deleted with the `rbtree_delete_node()` function. The tree can be traversed with the `rbtree_traverse_ascending()` and `rbtree_traverse_descending()` functions, which call a specified callback for each node in ascending or descending (by key) order. Subtrees can be removed from the tree with the `rbtree_delete()` function. Once the application code is finished with a tree, it can be freed with the `rbtree_free()` function.

See _main.c_ for usage of the library. All global declarations are in _rbtree.h_.

## Functions

`void rbtree_delete(rbtree_t *tree, rbtree_node_t *node)`
Delete the entire sub-tree structure rooted at _node_. If _node_ is _NULL_, the entire red-black _tree_ is deallocated and the _rbtree_t_ structure itself is deallocated. The specified _del_func_ is called for each node prior to its deletion (assuming a non-**NULL** _del_func_ was given).

`void rbtree_delete_node(rbtree_t *tree, rbtree_node_t *node)`
Delete a _node_ from the _tree_. The _del_func_, if not **NULL**, will be called with the given _node_ before the _node_ itself is deleted.

`void rbtree_free(rbtree_t *tree)`
Delete all nodes in the given _tree_ and frees memory allocated to the _tree_ structure.

`rbtree_node_t *rbtree_insert(rbtree_t *tree, void *key)`
Insert a new node with the given _key_ into the _tree_. The _cmp_func_ will be called to compare the given _key_ with the keys of other nodes in order to determine where the node should be inserted. If a node with this _key_ is already present in the _tree_, no new node is created and the pointer to **that** node is returned.

`rbtree_node_t *rbtree_lookup(rbtree_t *tree, void *key)`
Look up a node with the given _key_. If the node with a matching _key_ is found, a pointer to it is returned to the caller. The _cmp_func_ will be used to find the _key_ in the _tree_. If the _key_ is not found in the _tree_, **NULL** is returned.

`rbtree_node_t *rbtree_maximum(rbtree_t *tree, rbtree_node_t *subtree)`
Return the node with the key value with the highest ordinality rooted in the _subtree_. If the _tree_ is empty, **NULL** is returned. If _subtree_ is **NULL**, the root of the red-black _tree_ is used as the starting subtree.

`rbtree_node_t *rbtree_minimum(rbtree_t *tree, rbtree_node_t *subtree)`
Return the node with the key value with the lowest ordinality rooted in the _subtree_. If the _tree_ is empty, **NULL** is returned. If _subtree_ is **NULL**, the root of the red-black _tree_ is used as the starting subtree.

`rbtree_t *rbtree_new(rbtree_key_compare_func_t cmp_func, rbtree_node_delete_func_t del_func)`
Create a new red-black tree. The _cmp_func_ is a required function that compares two key values and returns **-1**, if _a_ is less than _b_, **0** if the two compared keys are equal, and **1** if _a_ is greater than _b_. The _del_func_ is an optional function that is called with a node just before it is deleted. This gives the caller an opportunity to delete memory allocated for the key and/or data belonging to the node.

`void rbtree_traverse_ascending(rbtree_t *tree, rbtree_node_t *subtree, rbtree_traverse_func_t cb)`
 Traverse a _subtree_ in order from lowest ordinal key to highest ordinal key If _subtree_ is **NULL**, then the traversal is across the entire _tree_. The specified callback function is called for every node visited, unless it is **NULL**, in which case this function is less than useful.

`void rbtree_traverse_descending(rbtree_t *tree, rbtree_node_t *subtree, rbtree_traverse_func_t cb)`
Traverse a _subtree_ in order from highest ordinal key to lowest ordinal key. If _subtree_ is _NULL_, then the traversal is across the entire _tree_. The specified callback function is called for every node  visited, unless it is _NULL_, in which case this function is less than useful.

## Types

    typedef struct rbtree_node_t {
        struct rbtree_node_t *parent;
        struct rbtree_node_t *left;
        struct rbtree_node_t *right;
        uint32_t flags;
        void *key;
        void *data;
    } rbtree_node_t

A red-black tree node structure. The `data` field is not referenced by the rbtree code in any way. The `parent`, `left` and `right` fields should not be altered by the application. The `key` field is set and used by the rbtree code internally, but it is the application's responsibility to free the `key` field in the `node_delete_func_t` that is called just prior to a node being deleted (if necessary). The `flags` field is used to track the "color" of the node. Only the most significant bit of `flags` is used internally, the other bits are not touched by any of the rbtree code, so they can be used as needed by the application. There are macros in rbtree.h to help in accessing the node color and the user flags.

    typedef int (*rbtree_key_compare_func_t)(void *a, void *b)

A function used to compare `rbtree_node_t` node keys. This function is required for properly ordering the nodes.

It should return a value less than **0** if _a_ is less than _b_, **0** if _a_ == _b_ or greater than **0** if _a_ > _b_.

    typedef void (*rbtree_node_delete_func_t)(rbtree_node_t *node)

A function that will receive a node just before it is deleted. This gives the owner the opportunity to free the `key` and `data` in the _node_, if necessary. If no memory or other cleanup needs to be done upon deletion of the _node_, this can be **NULL**.

    typedef void (*rbtree_traverse_func_t)(rbtree_node_t *node)
A function to call for each _node_ visited during a traversal.

    typedef struct rbtree_t {
        rbtree_node_t *root;
        rbtree_key_compare_func_t cmp_func;
        rbtree_node_delete_func_t del_func;
        rbtree_node_t nil_node;
        unsigned long int node_count;
    } rbtree_t;

A red-black tree. This structure tracks the tree `root` (which will change as nodes are added), the key comparison and node delete functions and keeps a special `nil_node` that is used internally for tree maintenance. `node_count` keeps an accurate count of the number of nodes in the tree as nodes are inserted and deleted.

## Constants

    #define RBTREE_COLOR_MASK
    #define RBTREE_USER_MASK
    #define RBTREE_COLOR_BLACK
    #define RBTREE_COLOR_RED

Flags for tracking the color of a node. The high-order bit of the flags field is used for color tracking. The remaining bits can be used by the application for whatever it wants and won't be modified by the rbtree code. _node->flags & RBTREE_COLOR_MASK_ gives the node color, while _node->flags & RBTREE_USER_MASK_ gives the user part of the flags for the node.

## Macros

    #define RBTREE_COLOR(n)
    #define RBTREE_COLOR_IS_BLACK(n)
    #define RBTREE_COLOR_IS_RED(n)
    #define RBTREE_SET_BLACK(n)
    #define RBTREE_SET_RED(n)
    #define RBTREE_SET_USER(n, d)
    #define RBTREE_GET_USER(n)

These are macros for working with the flags field. _RBTREE_COLOR(n)_ returns the color of node `n`. _RBTREE_COLOR_IS_BLACK(n)_ and _RBTREE_COLOR_IS_RED(n)_ return whether node `n` is black or red, respectively. _RBTREE_SET_BLACK(n)_ and _RBTREE_SET_RED(n)_ set node `n` to either black or red,
respectively, without modifying the user bits of `flags`. _RBTREE_SET_USER(n, d)_ will set the user portion of the `flags` belonging to node `n` to `d`, without modifying the color. The most significant bit of `d` (bit 31) is truncated and replaced with the current color of node `n`. Finally, _RBTREE_GET_USER(n)_ will retrieve the user data without the color bit.

## Word List

The test programs read in a list of 263,533 words from a file named _word.txt_. These words are used as keys for an rbtree. This list was obtained from this [website](https://norvig.com/ngrams/).

## Notes

- Compile/link with -lto to remove unused functions from the final executable.
