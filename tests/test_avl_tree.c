#include "./test_avl_tree.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include <string.h>

static int test_tree_init_free()
{
    tree_node *node = init_tree_node();
    ASSERT(node != NULL);
    free_tree(node);

    return 1;
}

#define MAX_NODES 100

static int test_tree_insert()
{
    tree_node *tree = init_tree_node();

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        tree_node *node = init_tree_node();
        node->payload = (void *) i;
        node->free_payload = NULL;
        insert_node(tree, node);
    }

  lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree_height(tree), MAX_NODES);
    free_tree(tree);
    return 1;
}

static int test_tree_insert_2()
{
    tree_node *tree = init_tree_node();

    // Add to tree
    for (size_t i = 0; i < MAX_NODES; i++) {
        tree_node *node = init_tree_node();
        node->payload = malloc(sizeof(int));
        node->free_payload = &free;
        insert_node(tree, node);
    }

  lprintf(LOG_INFO, "Tree height %lu for %lu node\n", tree_height(tree), MAX_NODES);
    free_tree(tree);
    return 1;
}

SUB_TEST(test_avl_tree, {&test_tree_init_free, "Test AVL tree init free"},
{&test_tree_insert, "Test tree insert"},
{&test_tree_insert_2, "Test tree insert 2"})
