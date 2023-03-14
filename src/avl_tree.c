#include "./avl_tree.h"
#include "../testing_h/testing.h"
#include <sys/param.h> // MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t __tree_height(tree_node *node)
{
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

void free_tree(tree_node *tree)
{
    if (tree->l) {
        free_tree(tree->l);
    }
    if (tree->r) {
        free_tree(tree->r);
    }

    if (tree->free_payload != NULL) {
        tree->free_payload(tree->payload);
    }
    free(tree);
}

int tree_balance(tree_node *root)
{
    if (!root) return 0;

    int lh, rh;
    lh = rh = 0;

    if (root->l) {
        lh = __tree_height(root->l);
    }
    if (root->r) {
        rh = __tree_height(root->r);
    }

    return lh - rh;
}

tree_node *init_tree_node(void (*free_payload)(void *payload),
                          int (*cmp_payload)(void *a, void *b),
                          void *payload)
{
    if (cmp_payload == NULL) {
        lprintf(LOG_ERROR, "Cannot have a NULL comparison function\n");
        return NULL;
    }

    tree_node *tree = malloc(sizeof * tree);
    if (tree == NULL) {
        lprintf(LOG_ERROR, "Cannot init tree node\n");
        return tree;
    }
    memset(tree, 0, sizeof * tree);
    tree->payload = payload;
    tree->cmp_payload = cmp_payload;
    tree->free_payload = free_payload;
    tree->height = 1;

    return tree;
}

static void __print_tree(tree_node *tree, int h)
{
    for (int i = 0; i < h; i++) {
        printf("  |");
    }
    printf("  |> %p %lu\n", tree->payload, tree->height);

    if (tree->l)  {
        __print_tree(tree->l, h + 1);
    }
    if (tree->r) {
        __print_tree(tree->r, h + 1);
    }
}

void print_tree(tree_node *root)
{
    __print_tree(root, 0);
}

static void __rotate_l(tree_node *root)
{
    /* Rotation (left):
      x            y
    y   c  ->    x   b
    a   b        a   c
     */
    void *tmp = root->l->payload;
    root->l->payload = root->payload;
    root->payload = tmp;

    tree_node *tmp2 = root->l->r;
    root->l->r = root->r;
    root->r = tmp2;

    // Update heights
    root->l->height = MAX(__tree_height(root->l->l),
                          __tree_height(root->l->r)) + 1;
    root->height = MAX(__tree_height(root->l),
                       __tree_height(root->r)) + 1;
}

static void __rotate_r(tree_node *root)
{
    /* Rotation (right):
       x            y
     a   y  ->    b   x
       b   c        a   c
     */
    void *tmp = root->r->payload;
    root->r->payload = root->payload;
    root->payload = tmp;

    tree_node *tmp2 = root->r->l;
    root->r->l = root->l;
    root->l = tmp2;

    // Update heights
    root->r->height = MAX(__tree_height(root->r->l),
                          __tree_height(root->r->r)) + 1;
    root->height = MAX(__tree_height(root->l),
                       __tree_height(root->r)) + 1;
}

void insert_node(tree_node *root, tree_node *node)
{
    root->height++;

    // BST insert
    if (root->cmp_payload(node->payload, root->payload) <= 0) {
        // Add left
        if (root->l) {
            insert_node(root->l, node);
        } else {
            root->l = node;
            return;
        }
    } else {
        // Add right
        if (root->r) {
            insert_node(root->r, node);
        } else {
            root->r = node;
            return;
        }
    }

    // Set height
    root->height = MAX(__tree_height(root->l),
                       __tree_height(root->r)) + 1;

    // Balance trees
    int balance = tree_balance(root);
    if (balance > 1) {
        if (tree_balance(root->l) > 0) {
            __rotate_l(root);
        } else {
            __rotate_l(root);
            __rotate_r(root);
        }
    } else if (balance < -1) {
        if (tree_balance(root->r) > 0) {
            __rotate_r(root);
            __rotate_l(root);
        } else {
            __rotate_r(root);
        }
    }
}