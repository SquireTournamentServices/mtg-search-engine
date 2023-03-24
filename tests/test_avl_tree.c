#include "./test_avl_tree.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include <string.h>
#include <time.h>

static int __test_heights(avl_tree_node_t *root, size_t h)
{
    if (root == NULL) {
        return 1;
    }

    ASSERT(root->height < h);
    ASSERT(__test_heights(root->l, h - 1));
    ASSERT(__test_heights(root->r, h - 1));
    return 1;
}

static int test_heights(avl_tree_node_t *root)
{
    return __test_heights(root, root->height + 1);
}

static int test_tree_props(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    if (node->l != NULL) {
        ASSERT(node->cmp_payload(node->l->payload, node->payload) <= 0);
        ASSERT(test_tree_props(node->l));
    }

    if (node->r != NULL) {
        ASSERT(node->cmp_payload(node->r->payload, node->payload) > 0);
        ASSERT(test_tree_props(node->r));
    }

    return 1;
}

static int cmp_size_t(void *a, void *b)
{
    if (a == b) {
        return 0;
    } else if (a < b) {
        return -1;
    }
    return 1;
}

static int test_tree_init_free()
{
    void *payload = (void *) 1L;
    avl_tree_node_t *node = init_avl_tree_node(NULL, &cmp_size_t, payload);
    ASSERT(node != NULL);
    ASSERT(node->height == 1);
    ASSERT(node->payload == payload);
    ASSERT(node->free_payload == NULL);
    ASSERT(node->cmp_payload == &cmp_size_t);
    ASSERT(find_payload(node, node->payload) == node);
    ASSERT(find_payload(node, payload) == node);
    ASSERT(tree_size(node) == 1);
    print_tree(node);
    free_tree(node);

    return 1;
}

#define MAX_NODES 10000

static int cmp_int_pointer(void *_a, void *_b)
{
    int a = *(int *) _a;
    int b = *(int *) _b;

    if (a == b) {
        return 0;
    } else if (a < b) {
        return -1;
    }
    return 1;
}

static int test_tree_insert()
{
    int *ptr = malloc(sizeof(*ptr));
    ASSERT(ptr != NULL);
    *ptr = -1;

    avl_tree_node_t *tree = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        int *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        *ptr = i;

        avl_tree_node_t *node = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
        ASSERT(node != NULL);
        ASSERT(node->payload == ptr);

        ASSERT(find_payload(tree, node->payload) == NULL);
        ASSERT(insert_node(&tree, node));
        ASSERT(find_payload(tree, node->payload) == node);
        ASSERT(tree_size(tree) == i + 2);
    }

    lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(tree_size(tree) == MAX_NODES + 1);

    free_tree(tree);
    return 1;
}

#define NODES_3 16

static int test_tree_insert_2()
{
    avl_tree_node_t *tree = init_avl_tree_node(NULL, &cmp_size_t,
                            (void *) (size_t) (NODES_3 + 5));
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < NODES_3; i++) {
        avl_tree_node_t *node = init_avl_tree_node(NULL, &cmp_size_t, (void *) i);
        ASSERT(node != NULL);
        ASSERT(find_payload(tree, node->payload) == NULL);
        ASSERT(insert_node(&tree, node));
        ASSERT(find_payload(tree, node->payload) == node);
    }

    lprintf(LOG_INFO, "Tree height %lu for %lu nodes\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(tree_size(tree) == NODES_3 + 1);
    print_tree(tree);

    free_tree(tree);
    return 1;
}

static int test_find_payload_null()
{
    ASSERT(find_payload(NULL, (void *) 1L) == NULL);
    return 1;
}

static int __assert_compare_node_lt(avl_tree_node_t *node, void *payload)
{
    if (node == NULL) {
        return 1;
    }

    ASSERT(node->cmp_payload(node->payload, payload) < 0);
    ASSERT(__assert_compare_node_lt(node->l, payload));
    ASSERT(__assert_compare_node_lt(node->r, payload));
    return 1;
}

static int __assert_compare_node_gt(avl_tree_node_t *node, void *payload)
{
    if (node == NULL) {
        return 1;
    }

    ASSERT(node->cmp_payload(node->payload, payload) > 0);
    ASSERT(__assert_compare_node_gt(node->l, payload));
    ASSERT(__assert_compare_node_gt(node->r, payload));
    return 1;
}

static int __assert_compare_node_lt_gt(avl_tree_node_t *node, void *ptr_1, void *ptr_2)
{
    if (node == NULL) {
        return 1;
    }

    ASSERT(node->cmp_payload(ptr_1, node->payload) < 0);
    ASSERT(node->cmp_payload(ptr_2, node->payload) > 0);
    ASSERT(__assert_compare_node_lt_gt(node->l, ptr_1, ptr_2));
    ASSERT(__assert_compare_node_lt_gt(node->r, ptr_1, ptr_2));
    return 1;
}

static int test_tree_lookup()
{
    int *ptr = malloc(sizeof(*ptr));
    ASSERT(ptr != NULL);
    *ptr = -1;

    avl_tree_node_t *tree = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 1; i < MAX_NODES; i++) {
        int *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        *ptr = i;

        avl_tree_node_t *node = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
        ASSERT(node != NULL);
        ASSERT(node->payload == ptr);

        ASSERT(insert_node(&tree, node));
    }

    // Sanity check
    int tmp = 3;
    int tmp2 = 3;
    ASSERT(tree->cmp_payload(&tmp, &tmp2) == 0);

    tmp = tmp2 - 1;
    ASSERT(tree->cmp_payload(&tmp, &tmp2) == -1);

    tmp = tmp2 + 2;
    ASSERT(tree->cmp_payload(&tmp, &tmp2) == 1);

    // Test lookup
    ptr = malloc(sizeof(*ptr));
    ASSERT(ptr != NULL);
    *ptr = MAX_NODES / 3;

    avl_tree_node_t *res = NULL;

    // Test less than
    ASSERT(tree_lookup(tree, &res, 1, ptr));
    ASSERT(res != NULL);

    lprintf(LOG_INFO, "%lu %d\n", tree_size(res), *ptr);
    ASSERT(tree_size(res) == (size_t) *ptr);

    ASSERT(res->free_payload == NULL);
    ASSERT(res->cmp_payload == tree->cmp_payload);
    ASSERT(__assert_compare_node_lt(res, ptr));
    free_tree(res);

    // Test greater than
    res = NULL;
    ASSERT(tree_lookup(tree, &res, 0, ptr));
    ASSERT(res != NULL);

    lprintf(LOG_INFO, "%lu %d\n", tree_size(res), MAX_NODES - *ptr - 1);
    ASSERT(tree_size(res) == (size_t) MAX_NODES - *ptr - 1);

    ASSERT(res->free_payload == NULL);
    ASSERT(res->cmp_payload == tree->cmp_payload);
    ASSERT(__assert_compare_node_gt(res, ptr));
    free_tree(res);

    free(ptr);

    // Test the tree is still valid
    lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(tree_size(tree) == MAX_NODES + 1);

    free_tree(tree);
    return 1;
}

static int test_tree_lookup_2()
{
    int *ptr = malloc(sizeof(*ptr));
    ASSERT(ptr != NULL);
    *ptr = -1;

    avl_tree_node_t *tree = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        int *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        *ptr = i;

        avl_tree_node_t *node = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
        ASSERT(node != NULL);
        ASSERT(node->payload == ptr);

        ASSERT(insert_node(&tree, node));
    }

    // Test lookup
    ptr = malloc(sizeof(*ptr));
    ASSERT(ptr != NULL);
    *ptr = MAX_NODES / 3;

    int *ptr_2 = malloc(sizeof(*ptr_2));
    ASSERT(ptr_2 != NULL);
    *ptr_2 = MAX_NODES / 2;

    avl_tree_node_t *res = NULL;
    ASSERT(tree_lookup_2(tree, &res, ptr, ptr_2));
    ASSERT(res != NULL);
    lprintf(LOG_INFO, "%lu %d\n", tree_size(res), *ptr_2 - *ptr - 1);
    ASSERT(tree_size(res) == (size_t) *ptr_2 - *ptr - 1);
    ASSERT(__assert_compare_node_lt_gt(res, ptr, ptr_2));
    free_tree(res);

    free(ptr);
    free(ptr_2);

    // Test the tree is still valid
    lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(tree_size(tree) == MAX_NODES + 1);

    free_tree(tree);
    return 1;
}

static int test_shallow_copy_tree_node()
{
    ASSERT(shallow_copy_tree_node(NULL) == NULL);

    avl_tree_node_t *tree = init_avl_tree_node(NULL, &cmp_size_t,
                            (void *) (size_t) (NODES_3 + 5));
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < NODES_3; i++) {
        avl_tree_node_t *node = init_avl_tree_node(NULL, &cmp_size_t, (void *) i);
        ASSERT(node != NULL);
        ASSERT(find_payload(tree, node->payload) == NULL);
        ASSERT(insert_node(&tree, node));
        ASSERT(find_payload(tree, node->payload) == node);
    }

    avl_tree_node_t *copy = shallow_copy_tree_node(tree);
    ASSERT(copy != tree);
    ASSERT(copy->l == NULL);
    ASSERT(copy->r == NULL);
    ASSERT(copy->height = 1);
    ASSERT(copy->payload == tree->payload);
    ASSERT(copy->free_payload == tree->free_payload);
    ASSERT(copy->cmp_payload == tree->cmp_payload);
    free_tree(copy);

    lprintf(LOG_INFO, "Tree height %lu for %lu nodes\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(tree_size(tree) == NODES_3 + 1);
    print_tree(tree);

    free_tree(tree);
    return 1;
}

SUB_TEST(test_avl_tree, {&test_tree_init_free, "Test AVL tree init free"},
{&test_tree_insert, "Test tree insert"},
{&test_tree_insert_2, "Test tree insert 2"},
{&test_find_payload_null, "Test find payload for NULL case"},
{&test_tree_lookup, "Test tree lookup"},
{&test_tree_lookup_2, "Test tree lookup 2"},
{&test_shallow_copy_tree_node, "Test shallow copy tree node"})
