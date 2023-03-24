#include "./test_search.h"
#include "../testing_h/testing.h"
#include "../src/search.h"
#include "../src/avl_tree.h"
#include "../src/card.h"
#include "../src/uuid.h"
#include <string.h>
#include <stdlib.h>

static avl_tree_node_t *get_tree(int min, int max)
{
    avl_tree_node_t *tree = NULL;
    for (int i = min; i <= max; i++) {
        mtg_card_t *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        memset(ptr, 0, sizeof(*ptr));
        memset(ptr->id.bytes, i, sizeof(*ptr->id.bytes));

        avl_tree_node_t *node = init_avl_tree_node(&free, &avl_cmp_card, (void *) ptr);
        ASSERT(node != NULL);
        ASSERT(insert_node(&tree, node));
    }
    return tree;
}

static int test_union_tree()
{
    // Init trees
    avl_tree_node_t *tree_1 = get_tree(1, 10);
    ASSERT(tree_size(tree_1) == 10);
    ASSERT(tree_1 != NULL);
    mse_search_intermediate_t a = init_mse_search_intermediate_tree(tree_1);

    avl_tree_node_t *tree_2 = get_tree(11, 20);
    ASSERT(tree_size(tree_2) == 10);
    ASSERT(tree_2 != NULL);
    mse_search_intermediate_t b = init_mse_search_intermediate_tree(tree_2);

    // Test set operations
    mse_search_intermediate_t inter;
    ASSERT(mse_set_union(&inter, &a, &b));

    ASSERT(inter.node != NULL);
    ASSERT(tree_size(inter.node) == tree_size(a.node) + tree_size(b.node));

    free_mse_search_intermediate(&inter);

    free_tree(tree_1);
    free_tree(tree_2);

    return 1;
}

static int test_intersection_tree()
{
    // Init trees
    avl_tree_node_t *tree_1 = get_tree(1, 100);
    ASSERT(tree_size(tree_1) == 100);
    ASSERT(tree_1 != NULL);
    mse_search_intermediate_t a = init_mse_search_intermediate_tree(tree_1);

    avl_tree_node_t *tree_2 = get_tree(90, 200);
    ASSERT(tree_size(tree_2) == 111);
    ASSERT(tree_2 != NULL);
    mse_search_intermediate_t b = init_mse_search_intermediate_tree(tree_2);

    // Test set operations
    mse_search_intermediate_t inter;
    ASSERT(mse_set_intersection(&inter, &a, &b));

    ASSERT(inter.node != NULL);
    ASSERT(tree_size(inter.node) == 11);

    free_mse_search_intermediate(&inter);

    free_tree(tree_1);
    free_tree(tree_2);

    return 1;

}

SUB_TEST(test_search, {&test_union_tree, "Test set union on trees"},
{&test_intersection_tree, "Test set intersection on trees"})
