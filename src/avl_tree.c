#include "./avl_tree.h"
#include "../testing_h/testing.h"
#include <sys/param.h> // MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t __tree_height(avl_tree_node *node)
{
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

void free_tree(avl_tree_node *tree)
{
    if (tree == NULL) {
        return;
    }
    free_tree(tree->l);
    free_tree(tree->r);

    if (tree->free_payload != NULL) {
        tree->free_payload(tree->payload);
    }
    free(tree);
}

int tree_balance(avl_tree_node *root)
{
    if (root == NULL) {
        return 0;
    }

    int lh = __tree_height(root->l);
    int rh = __tree_height(root->r);

    return lh - rh;
}

avl_tree_node *init_avl_tree_node(void (*free_payload)(void *payload),
                                  int (*cmp_payload)(void *a, void *b),
                                  void *payload)
{
    if (cmp_payload == NULL) {
        lprintf(LOG_ERROR, "Cannot have a NULL comparison function\n");
        return NULL;
    }

    avl_tree_node *tree = malloc(sizeof * tree);
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

static void __print_tree(avl_tree_node *tree, int h)
{
    for (int i = 0; i < h; i++) {
        printf("  |");
    }
    printf("  |> %p %lu\n", tree->payload, tree->height);

    if (tree->l)  {
        __print_tree(tree->l, h + 1);
    } else {
        puts("--");
    }

    if (tree->r) {
        __print_tree(tree->r, h + 1);
    } else {
        puts("--");
    }
}

void print_tree(avl_tree_node *root)
{
    __print_tree(root, 0);
}

static void __rotate_update_heights(avl_tree_node *root)
{
    if (root->r != NULL) {
        root->r->height = MAX(__tree_height(root->r->l),
                              __tree_height(root->r->r)) + 1;
    }

    if (root->l != NULL) {
        root->l->height = MAX(__tree_height(root->l->l),
                              __tree_height(root->l->r)) + 1;
    }
    root->height = MAX(__tree_height(root->l),
                       __tree_height(root->r)) + 1;
}

static avl_tree_node *__rotate_r(avl_tree_node *y)
{
    /* Rotation (right):
    *      x          y
    *    y   c  ->  a   x
    *  a   b          b   c
    **/
    avl_tree_node *x = y->l;
    avl_tree_node *t2 = x->r;

    // Perform rotation
    x->r = y;
    y->l = t2;

    __rotate_update_heights(x);
    return x;
}

static avl_tree_node *__rotate_l(avl_tree_node *x)
{
    /* Rotation (left):
    *    x              y
    *  a   y    ->    a   x
    *    b   c      b   c
    **/
    avl_tree_node *y = x->r;
    avl_tree_node *t2 = y->l;

    // Perform rotation
    y->l = x;
    x->r = t2;

    __rotate_update_heights(y);
    return y;
}

static int __cmp_payload(avl_tree_node *a, avl_tree_node *b)
{
    if (a == NULL || b == NULL) {
        return 0;
    } else {
        return a->cmp_payload(a->payload, b->payload);
    }
}

static avl_tree_node *__do_insert_node(avl_tree_node *root, avl_tree_node *node)
{
    // Base case
    if (root == NULL) {
        return node;
    }

    // BST insert
    int cmp = __cmp_payload(node, root);

    // cmp != 0 as insert_node checks this
    if (cmp <= 0) {
        // Add left
        root->l = __do_insert_node(root->l, node);
    } else {
        // Add right
        root->r = __do_insert_node(root->r, node);
    }

    // Set height
    root->height = MAX(__tree_height(root->l),
                       __tree_height(root->r)) + 1;

    // Balance trees
    int balance = tree_balance(root);

    if (balance > 1 && __cmp_payload(node, root->l) < 0) {
        return __rotate_r(root);
    } else if (balance < -1 && __cmp_payload(node, root->r) > 0) {
        return __rotate_l(root);
    } else if (balance > 1 && __cmp_payload(node, root->l) > 0) {
        root->l = __rotate_l(root->l);
        return __rotate_r(root);
    } else if (balance < -1 && __cmp_payload(node, root->r) < 0) {
        root->r = __rotate_r(root->r);
        return __rotate_l(root);
    }

    return root;
}

int insert_node(avl_tree_node **root, avl_tree_node *node)
{
    ASSERT(root != NULL);
    ASSERT(node != NULL);
    if (find_payload(*root, node->payload)) {
        return 0;
    }
    *root = __do_insert_node(*root, node);
    return 1;
}

avl_tree_node *find_payload(avl_tree_node *node, void *payload)
{
    if (node == NULL) {
        return NULL;
    }

    int cmp = node->cmp_payload(payload, node->payload);
    if (cmp == 0) {
        return node;
    } else if (cmp < 0) {
        return find_payload(node->l, payload);
    } else {
        return find_payload(node->r, payload);
    }
}
