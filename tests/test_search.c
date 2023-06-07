#include "./test_search.h"
#include "../testing_h/testing.h"
#include "../mse/search.h"
#include "../mse/avl_tree.h"
#include "../mse/card.h"
#include "../mse/uuid.h"
#include "../mse/mtg_json.h"
#include <string.h>
#include <stdlib.h>

static avl_tree_node_t *get_tree(int min, int max)
{
    avl_tree_node_t *tree = NULL;
    for (int i = min; i <= max; i++) {
        mse_card_t *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        memset(ptr, 0, sizeof(*ptr));
        memset(ptr->id.bytes, i, sizeof(*ptr->id.bytes));

        avl_tree_node_t *node = init_avl_tree_node(&free, &mse_avl_cmp_card, (void *) ptr);
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
    mse_search_intermediate_t a = mse_init_search_intermediate_tree(tree_1, 0);

    avl_tree_node_t *tree_2 = get_tree(11, 20);
    ASSERT(tree_size(tree_2) == 10);
    ASSERT(tree_2 != NULL);
    mse_search_intermediate_t b = mse_init_search_intermediate_tree(tree_2, 0);

    // Test set operations
    mse_search_intermediate_t inter;
    ASSERT(mse_set_union(&inter, &a, &b));

    ASSERT(inter.node != NULL);
    ASSERT(tree_size(inter.node) == tree_size(a.node) + tree_size(b.node));

    mse_free_search_intermediate(&inter);

    free_tree(tree_1);
    free_tree(tree_2);
    return 1;
}

/// This is also a test for is_reference, might want to move this elsewhere tbh
static int test_intersection_tree()
{
    // Init trees
    avl_tree_node_t *tree_1 = get_tree(1, 100);
    ASSERT(tree_size(tree_1) == 100);
    ASSERT(tree_1 != NULL);
    mse_search_intermediate_t a = mse_init_search_intermediate_tree(tree_1, 1);

    avl_tree_node_t *tree_2 = get_tree(90, 200);
    ASSERT(tree_size(tree_2) == 111);
    ASSERT(tree_2 != NULL);
    mse_search_intermediate_t b = mse_init_search_intermediate_tree(tree_2, 1);

    // Test set operations
    mse_search_intermediate_t inter;
    ASSERT(mse_set_intersection(&inter, &a, &b));

    ASSERT(inter.node != NULL);
    ASSERT(tree_size(inter.node) == 11);

    mse_free_search_intermediate(&inter);
    mse_free_search_intermediate(&a);
    mse_free_search_intermediate(&b);

    free_tree(tree_1);
    free_tree(tree_2);
    return 1;
}

static int test_set_negation()
{
    // Setup mock cards
    mse_all_printings_cards_t cards;
    memset(&cards, 0, sizeof(cards));

    mse_card_t card_a, card_b;
    memset(&card_a, 0, sizeof(card_a));
    card_b = card_a;

    card_a.name = "Johnny";
    memset(&card_a.id.bytes, 1, sizeof(card_a.id.bytes));

    card_b.name = "Bing";
    memset(&card_b.id.bytes, 2, sizeof(card_b.id.bytes));

    // Insert the cards
    avl_tree_node_t *node = init_avl_tree_node(NULL, &mse_avl_cmp_card, &card_a);
    ASSERT(node);
    ASSERT(insert_node(&cards.card_tree, node));

    node = init_avl_tree_node(NULL, &mse_avl_cmp_card, &card_b);
    ASSERT(node);
    ASSERT(insert_node(&cards.card_tree, node));
    ASSERT(tree_size(cards.card_tree) == 2);

    // Setup and get the results
    mse_search_intermediate_t ret, a;
    memset(&ret, 0, sizeof(ret));
    a = ret;

    // Negation of empty set should be full
    ASSERT(mse_set_negate(&ret, &cards, &a));
    ASSERT(tree_size(ret.node) == 2);
    mse_free_search_intermediate(&ret);

    // Negation of entire set should be empty
    a.node = cards.card_tree;
    ASSERT(mse_set_negate(&ret, &cards, &a));
    ASSERT(ret.node == NULL);
    mse_free_search_intermediate(&ret);
    a.node = NULL;

    // Negation of a set with one item should return a set with the other
    node = init_avl_tree_node(NULL, &mse_avl_cmp_card, &card_a);
    ASSERT(node);
    a.node = node;

    ASSERT(mse_set_negate(&ret, &cards, &a));
    ASSERT(tree_size(ret.node) == 1);
    ASSERT(ret.node->payload == &card_b);

    mse_free_search_intermediate(&ret);
    mse_free_search_intermediate(&a);

    // Cleanup
    free_tree(cards.card_tree);
    return 1;
}

SUB_TEST(test_search, {&test_union_tree, "Test set union on trees"},
{&test_intersection_tree, "Test set intersection on trees"},
{&test_set_negation, "Test set negation"})
