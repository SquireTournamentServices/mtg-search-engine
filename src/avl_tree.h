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

int insert_node(avl_tree_node *root, avl_tree_node *node);

/// Returns 1 if the payload is in the tree, otherwise 0
int find_payload(avl_tree_node *node, void *payload);
