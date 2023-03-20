#pragma once
#include <stddef.h>

// based on an AVL tree I made earlier
// https://github.com/djpiper28/cs2860-implemented-algs/blob/main/avl%20tree/tree.c

typedef struct avl_tree_node {
    struct avl_tree_node *l;
    struct avl_tree_node *r;
    void *payload;
    void (*free_payload)(void *payload);
    int (*cmp_payload)(void *a, void *b);
    size_t height;
} avl_tree_node;

avl_tree_node *init_avl_tree_node(void (*free_payload)(void *payload),
                                  int (*cmp_payload)(void *a, void *b),
                                  void *payload);
int tree_balance(avl_tree_node *node);
void free_tree(avl_tree_node *tree);
void print_tree(avl_tree_node *root);

int insert_node(avl_tree_node **root, avl_tree_node *node);

/// Returns 1 if the payload is in the tree, otherwise 0
avl_tree_node *find_payload(avl_tree_node *node, void *payload);

typedef struct avl_tree_lookup_t {
    size_t results_length;
    void **results;
} avl_tree_lookup_t;

/// Lookup for nodes that are less than (or greater than) cmp_payload
int tree_lookup(avl_tree_node *root, avl_tree_lookup_t *res, int less_than, void *cmp_payload);

void free_tree_lookup(avl_tree_lookup_t *res);
