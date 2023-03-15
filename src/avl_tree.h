#pragma once
#include <stddef.h>

// based on an AVL tree I made earlier
// https://github.com/djpiper28/cs2860-implemented-algs/blob/main/avl%20tree/tree.c

typedef struct tree_node {
    struct tree_node *l;
    struct tree_node *r;
    void *payload;
    void (*free_payload)(void *payload);
    int (*cmp_payload)(void *a, void *b);
    size_t height;
} tree_node;

tree_node *init_tree_node(void (*free_payload)(void *payload),
                          int (*cmp_payload)(void *a, void *b),
                          void *payload);
int tree_balance(tree_node *node);
void free_tree(tree_node *tree);
void print_tree(tree_node *root);

int insert_node(tree_node *root, tree_node *node);

/// Returns 1 if the payload is in the tree, otherwise 0
int find_payload(tree_node *node, void *payload);
