#include "./test_avl_tree.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include <string.h>

static int cmp_size_t(void *_a, void *_b)
{
    size_t a = (size_t) _a;
    size_t b = (size_t) _b;

    if (a == b) {
        return 0;
    } else if (a < b) {
        return -1;
    }
    return 1;
}

static int test_tree_init_free()
{
    tree_node *node = init_tree_node(NULL, &cmp_size_t, (void *) 1L);
    ASSERT(node != NULL);
    free_tree(node);

    return 1;
}

#define MAX_NODES 10000

static int test_tree_insert()
{
    tree_node *tree = init_tree_node(NULL, &cmp_size_t, (void *) 1L);
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        tree_node *node = init_tree_node(NULL, &cmp_size_t, (void *) random());
        ASSERT(node != NULL);
        insert_node(tree, node);
    }

    lprintf(LOG_INFO, "Tree height %lu for %lu nodes\n", tree_height(tree), MAX_NODES);
    free_tree(tree);
    return 1;
}

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

static int test_tree_insert_2()
{
    int *ptr = malloc(sizeof(*ptr));
    ASSERT(ptr != NULL);
    *ptr = random();

    tree_node *tree = init_tree_node(&free, &cmp_int_pointer, ptr);
    ASSERT(tree != NULL);

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        int *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        *ptr = random();

        tree_node *node = init_tree_node(&free, &cmp_int_pointer, ptr);
        ASSERT(node != NULL);
        insert_node(tree, node);
    }

    lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree_height(tree), MAX_NODES);
    free_tree(tree);
    return 1;
}

SUB_TEST(test_avl_tree, {&test_tree_init_free, "Test AVL tree init free"},
{&test_tree_insert, "Test tree insert"},
{&test_tree_insert_2, "Test tree insert 2"})
