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

mse_avl_tree_node_t *mse_init_avl_tree_node(void (*free_payload)(void *payload),
        int (*cmp_payload)(void *a, void *b),
        void *payload);

int mse_tree_balance(mse_avl_tree_node_t *node);
void mse_free_tree(mse_avl_tree_node_t *tree);
void mse_print_tree(mse_avl_tree_node_t *root);

int mse_insert_node(mse_avl_tree_node_t **root, mse_avl_tree_node_t *node);

/// Returns a ptr if the payload is in the tree, otherwise NULL
mse_avl_tree_node_t *mse_find_payload(mse_avl_tree_node_t *node, void *payload);

/// Returns how many elements are in a tree
size_t mse_tree_size(mse_avl_tree_node_t *node);

/// Copies the contents of the node
mse_avl_tree_node_t *mse_shallow_copy_tree_node(mse_avl_tree_node_t *node);

/// Lookup for nodes that are less than (or greater than) cmp_payload
int mse_tree_lookup(mse_avl_tree_node_t *root, mse_avl_tree_node_t **res, int less_than, void *cmp_payload);
int mse_tree_lookup_2(mse_avl_tree_node_t *root, mse_avl_tree_node_t **res, void *lower, void *upper);
