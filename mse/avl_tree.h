#pragma once
#include <stddef.h>

// based on an AVL tree I made earlier
// https://github.com/djpiper28/cs2860-implemented-algs/blob/main/avl%20tree/tree.c

typedef struct mse_avl_tree_node_t {
    struct mse_avl_tree_node_t *l;
    struct mse_avl_tree_node_t *r;
    void *payload;
    void (*free_payload)(void *payload);
    int (*cmp_payload)(void *a, void *b);
    size_t height;
} mse_avl_tree_node_t;

extern mse_avl_tree_node_t *mse_init_avl_tree_node(void (*free_payload)(void *payload),
        int (*cmp_payload)(void *a, void *b),
        void *payload);
extern int mse_tree_balance(mse_avl_tree_node_t *node);
extern void mse_free_tree(mse_avl_tree_node_t *tree);
extern void mse_print_tree(mse_avl_tree_node_t *root);

extern int mse_insert_node(mse_avl_tree_node_t **root, mse_avl_tree_node_t *node);

/// Returns 1 if the payload is in the tree, otherwise 0
extern mse_avl_tree_node_t *mse_find_payload(mse_avl_tree_node_t *node, void *payload);

/// Returns how many elements are in a tree
extern size_t mse_tree_size(mse_avl_tree_node_t *node);

/// Copies the contents of the node
extern mse_avl_tree_node_t *mse_shallow_copy_tree_node(mse_avl_tree_node_t *node);

/// Lookup for nodes that are less than (or greater than) cmp_payload
extern int mse_tree_lookup(mse_avl_tree_node_t *root, mse_avl_tree_node_t **res, int less_than, void *cmp_payload);
extern int mse_tree_lookup_2(mse_avl_tree_node_t *root, mse_avl_tree_node_t **res, void *lower, void *upper);
