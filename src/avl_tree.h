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
} tree_node;

tree_node *init_tree_node(void (*free_payload)(void *payload),
                          int (*cmp_payload)(void *a, void *b),
                          void *payload);
size_t tree_height(tree_node *node);
int tree_balance(tree_node *node);
void free_tree(tree_node *tree);
void print_tree(tree_node *root);

void insert_node(tree_node *root, tree_node *node);
