#pragma once
#include <stddef.h>

// based on an AVL tree I made earlier
// https://github.com/djpiper28/cs2860-implemented-algs/blob/main/avl%20tree/tree.c

typedef struct avl_tree_node_t {
    struct avl_tree_node_t *l;
    struct avl_tree_node_t *r;
    void *payload;
    void (*free_payload)(void *payload);
    int (*cmp_payload)(void *a, void *b);
    size_t height;
} avl_tree_node_t;

avl_tree_node_t *init_avl_tree_node(void (*free_payload)(void *payload),
                                    int (*cmp_payload)(void *a, void *b),
                                    void *payload);
int tree_balance(avl_tree_node_t *node);
void free_tree(avl_tree_node_t *tree);
void print_tree(avl_tree_node_t *root);

int insert_node(avl_tree_node_t **root, avl_tree_node_t *node);

/// Returns 1 if the payload is in the tree, otherwise 0
avl_tree_node_t *find_payload(avl_tree_node_t *node, void *payload);

/// Returns how many elements are in a tree
size_t tree_size(avl_tree_node_t *node);

/// Copies the contents of the node
avl_tree_node_t *shallow_copy_tree_node(avl_tree_node_t *node);

/// Lookup for nodes that are less than (or greater than) cmp_payload
int tree_lookup(avl_tree_node_t *root, avl_tree_node_t **res, int less_than, void *cmp_payload);
int tree_lookup_2(avl_tree_node_t *root, avl_tree_node_t **res, void *lower, void *upper);
