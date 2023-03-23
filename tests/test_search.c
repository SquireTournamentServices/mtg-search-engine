#include "./test_search.h"
#include "../testing_h/testing.h"
#include "../src/search.h"
#include "../src/avl_tree.h"
#include <stdlib.h>

static int cmp_payload(void *_a, void *_b)
{
    int *a = (int *) _a;
    int *b = (int *) _b;

    if (*a == *b) {
        return 0;
    } else if (*a < *b) {
        return -1;
    }
    return 1;
}

static avl_tree_node *get_tree(int min, int max)
{
    avl_tree_node *tree = NULL;
    for (int i = min; i <= max; i++) {
        int *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        *ptr = i;

        avl_tree_node *node = init_avl_tree_node(&free, &cmp_payload, (void *) ptr);
        ASSERT(node != NULL);
        ASSERT(insert_node(&tree, node));
    }
    return tree;
}

static int test_union()
{
    avl_tree_node *tree_1 = get_tree(1, 10);
    ASSERT(tree_size(tree_1) == 10);
    ASSERT(tree_1 != NULL);
    mse_search_intermediate_t a = init_mse_search_intermediate_tree(tree_1);

    avl_tree_node *tree_2 = get_tree(11, 20);
    ASSERT(tree_size(tree_2) == 10);
    ASSERT(tree_2 != NULL);
    mse_search_intermediate_t b = init_mse_search_intermediate_tree(tree_2);

    mse_search_intermediate_t inter;
    ASSERT(mse_set_union(&inter, &a, &b));

    ASSERT(inter.type == MSE_INTERMEDIATE_TREE);
    ASSERT(inter.node != NULL);
    ASSERT(tree_size(inter.node) == tree_size(a.node) + tree_size(b.node));

    // tree_1 and tree_2 are now owned by a and b
    free_mse_search_intermediate(&inter);
    free_mse_search_intermediate(&a);
    free_mse_search_intermediate(&b);

    return 1;
}

SUB_TEST(test_search, {&test_union, "Test set union"})
