#include "./test_avl_tree.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include <string.h>
#include <time.h>

static int __test_heights(avl_tree_node *root, size_t h)
{
    if (root == NULL) {
        return 1;
    }

    ASSERT(root->height < h);
    ASSERT(__test_heights(root->l, h - 1));
    ASSERT(__test_heights(root->r, h - 1));
    return 1;
}

static int test_heights(avl_tree_node *root)
{
    return __test_heights(root, root->height + 1);
}

static size_t get_tree_nodes(avl_tree_node *node)
{
    if (node == NULL) {
        return 0;
    }
    return 1 + get_tree_nodes(node->l) + get_tree_nodes(node->r);
}

static int test_tree_props(avl_tree_node *node)
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
    avl_tree_node *node = init_avl_tree_node(NULL, &cmp_size_t, payload);
    ASSERT(node != NULL);
    ASSERT(node->height == 1);
    ASSERT(node->payload == payload);
    ASSERT(node->free_payload == NULL);
    ASSERT(node->cmp_payload == &cmp_size_t);
    ASSERT(find_payload(node, node->payload) == node);
    ASSERT(find_payload(node, payload) == node);
    ASSERT(get_tree_nodes(node) == 1);
    print_tree(node);
    free_tree(node);

    return 1;
}

#define MAX_NODES 10000
#define MAX_TIME 3

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
    time_t t1 = time(NULL);
    int *ptr = malloc(sizeof(*ptr));
    ASSERT(ptr != NULL);
    *ptr = random();

    avl_tree_node *tree = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        int *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        *ptr = i;

        avl_tree_node *node = init_avl_tree_node(&free, &cmp_int_pointer, ptr);
        ASSERT(node != NULL);
        ASSERT(node->payload == ptr);

        ASSERT(find_payload(tree, node->payload) == NULL);
        ASSERT(insert_node(&tree, node));
        ASSERT(find_payload(tree, node->payload) == node);
        ASSERT(get_tree_nodes(tree) == i + 2);
    }

    time_t t2 = time(NULL);
    ASSERT(t2 - t1 <= MAX_TIME);

    lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(get_tree_nodes(tree) == MAX_NODES + 1);

    free_tree(tree);
    return 1;
}

#define NODES_3 16

static int test_tree_insert_2()
{
    time_t t1 = time(NULL);
    avl_tree_node *tree = init_avl_tree_node(NULL, &cmp_size_t,
                          (void *) (size_t) (NODES_3 + 5));
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < NODES_3; i++) {
        avl_tree_node *node = init_avl_tree_node(NULL, &cmp_size_t, (void *) i);
        ASSERT(node != NULL);
        ASSERT(find_payload(tree, node->payload) == NULL);
        ASSERT(insert_node(&tree, node));
        ASSERT(find_payload(tree, node->payload) == node);
    }

    time_t t2 = time(NULL);
    ASSERT(t2 - t1 <= MAX_TIME);

    lprintf(LOG_INFO, "Tree height %lu for %lu nodes\n", tree->height, MAX_NODES);
    ASSERT(test_heights(tree));
    ASSERT(test_tree_props(tree));
    ASSERT(get_tree_nodes(tree) == NODES_3 + 1);
    print_tree(tree);

    free_tree(tree);
    return 1;
}

static int test_find_payload_null()
{
    ASSERT(find_payload(NULL, (void *) 1L) == NULL);
    return 1;
}

SUB_TEST(test_avl_tree, {&test_tree_init_free, "Test AVL tree init free"},
{&test_tree_insert, "Test tree insert"},
{&test_tree_insert_2, "Test tree insert 2"},
{&test_find_payload_null, "Test find payload for NULL case"})
