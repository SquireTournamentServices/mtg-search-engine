#include "./test_search.h"
#include "../testing_h/testing.h"
#include "../mse/search.h"
#include "../mse/avl_tree.h"
#include "../mse/card.h"
#include "../mse/uuid.h"
#include "../mse/mtg_json.h"
#include <string.h>
#include <stdlib.h>

static mse_avl_tree_node_t *get_tree(int min, int max)
{
    mse_avl_tree_node_t *tree = NULL;
    for (int i = min; i <= max; i++) {
        mse_card_t *ptr = malloc(sizeof(*ptr));
        ASSERT(ptr != NULL);
        memset(ptr, 0, sizeof(*ptr));
        memset(ptr->id.bytes, i, sizeof(*ptr->id.bytes));

        mse_avl_tree_node_t *node = mse_init_avl_tree_node(&free, &mse_avl_cmp_card, (void *) ptr);
        ASSERT(node != NULL);
        ASSERT(mse_insert_node(&tree, node));
    }
    return tree;
}

static int test_union_tree()
{
    // Init trees
    mse_avl_tree_node_t *tree_1 = get_tree(1, 10);
    ASSERT(mse_tree_size(tree_1) == 10);
    ASSERT(tree_1 != NULL);
    mse_search_intermediate_t a = mse_init_search_intermediate_tree(tree_1, 0);

    mse_avl_tree_node_t *tree_2 = get_tree(11, 20);
    ASSERT(mse_tree_size(tree_2) == 10);
    ASSERT(tree_2 != NULL);
    mse_search_intermediate_t b = mse_init_search_intermediate_tree(tree_2, 0);

    // Test set operations
    mse_search_intermediate_t inter;
    ASSERT(mse_set_union(&inter, &a, &b));

    ASSERT(inter.node != NULL);
    ASSERT(mse_tree_size(inter.node) == mse_tree_size(a.node) + mse_tree_size(b.node));

    mse_free_search_intermediate(&inter);

    mse_free_tree(tree_1);
    mse_free_tree(tree_2);
    return 1;
}

/// This is also a test for is_reference, might want to move this elsewhere tbh
static int test_intersection_tree()
{
    // Init trees
    mse_avl_tree_node_t *tree_1 = get_tree(1, 100);
    ASSERT(mse_tree_size(tree_1) == 100);
    ASSERT(tree_1 != NULL);
    mse_search_intermediate_t a = mse_init_search_intermediate_tree(tree_1, 1);

    mse_avl_tree_node_t *tree_2 = get_tree(90, 200);
    ASSERT(mse_tree_size(tree_2) == 111);
    ASSERT(tree_2 != NULL);
    mse_search_intermediate_t b = mse_init_search_intermediate_tree(tree_2, 1);

    // Test set operations
    mse_search_intermediate_t inter;
    ASSERT(mse_set_intersection(&inter, &a, &b));

    ASSERT(inter.node != NULL);
    ASSERT(mse_tree_size(inter.node) == 11);

    mse_free_search_intermediate(&inter);
    mse_free_search_intermediate(&a);
    mse_free_search_intermediate(&b);

    mse_free_tree(tree_1);
    mse_free_tree(tree_2);
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
    mse_avl_tree_node_t *node = mse_init_avl_tree_node(NULL, &mse_avl_cmp_card, &card_a);
    ASSERT(node);
    ASSERT(mse_insert_node(&cards.card_tree, node));

    node = mse_init_avl_tree_node(NULL, &mse_avl_cmp_card, &card_b);
    ASSERT(node);
    ASSERT(mse_insert_node(&cards.card_tree, node));
    ASSERT(mse_tree_size(cards.card_tree) == 2);

    // Setup and get the results
    mse_search_intermediate_t ret, a;
    memset(&ret, 0, sizeof(ret));
    a = ret;

    // Negation of empty set should be full
    ASSERT(mse_set_negate(&ret, &cards, &a));
    ASSERT(mse_tree_size(ret.node) == 2);
    mse_free_search_intermediate(&ret);

    // Negation of entire set should be empty
    a.node = cards.card_tree;
    ASSERT(mse_set_negate(&ret, &cards, &a));
    ASSERT(ret.node == NULL);
    mse_free_search_intermediate(&ret);
    a.node = NULL;

    // Negation of a set with one item should return a set with the other
    node = mse_init_avl_tree_node(NULL, &mse_avl_cmp_card, &card_a);
    ASSERT(node);
    a.node = node;

    ASSERT(mse_set_negate(&ret, &cards, &a));
    ASSERT(mse_tree_size(ret.node) == 1);
    ASSERT(ret.node->payload == &card_b);

    mse_free_search_intermediate(&ret);
    mse_free_search_intermediate(&a);

    // Cleanup
    mse_free_tree(cards.card_tree);
    return 1;
}

static int test_sort_same_as_current_sort()
{
    // Setup mock cards
    mse_card_t card_a, card_b;
    memset(&card_a, 0, sizeof(card_a));
    card_b = card_a;

    card_a.name = "Johnny";
    memset(&card_a.id.bytes, 1, sizeof(card_a.id.bytes));

    card_b.name = "Bing";
    memset(&card_b.id.bytes, 2, sizeof(card_b.id.bytes));

    mse_search_result_t cards;
    memset(&cards, 0, sizeof(cards));
    cards.current_sort = MSE_SORT_END;

    cards.cards_length = 2;
    ASSERT(cards.cards = malloc(sizeof(cards.cards) * cards.cards_length));

    cards.cards[0] = &card_a;
    cards.cards[1] = &card_b;

    // Sort
    mse_sort_search_results(&cards, MSE_SORT_CARD_NAME);
    ASSERT(mse_avl_cmp_card_name(cards.cards[0], cards.cards[1]) < 0);

    mse_sort_search_results(&cards, MSE_SORT_CARD_NAME);

    ASSERT(mse_avl_cmp_card_name(cards.cards[0], cards.cards[1]) < 0);

    free(cards.cards);
    return 1;
}

static int test_sort_card_name()
{
    // Setup mock cards
    mse_card_t card_a, card_b;
    memset(&card_a, 0, sizeof(card_a));
    card_b = card_a;

    card_a.name = "Johnny";
    memset(&card_a.id.bytes, 1, sizeof(card_a.id.bytes));

    card_b.name = "Bing";
    memset(&card_b.id.bytes, 2, sizeof(card_b.id.bytes));

    mse_search_result_t cards;
    memset(&cards, 0, sizeof(cards));
    cards.current_sort = MSE_SORT_END;

    cards.cards_length = 2;
    ASSERT(cards.cards = malloc(sizeof(cards.cards) * cards.cards_length));

    cards.cards[0] = &card_a;
    cards.cards[1] = &card_b;

    // Sort
    mse_sort_search_results(&cards, MSE_SORT_CARD_NAME);
    ASSERT(cards.current_sort == MSE_SORT_CARD_NAME);
    ASSERT(mse_avl_cmp_card_name(cards.cards[0], cards.cards[1]) < 0);

    free(cards.cards);
    return 1;
}

static int test_sort_card_cmc()
{
    // Setup mock cards
    mse_card_t card_a, card_b;
    memset(&card_a, 0, sizeof(card_a));
    card_b = card_a;

    card_a.cmc = 1;
    memset(&card_a.id.bytes, 1, sizeof(card_a.id.bytes));

    card_b.cmc = 123;
    memset(&card_b.id.bytes, 2, sizeof(card_b.id.bytes));

    mse_search_result_t cards;
    memset(&cards, 0, sizeof(cards));
    cards.current_sort = MSE_SORT_END;

    cards.cards_length = 2;
    ASSERT(cards.cards = malloc(sizeof(cards.cards) * cards.cards_length));

    cards.cards[0] = &card_a;
    cards.cards[1] = &card_b;

    // Sort
    mse_sort_search_results(&cards, MSE_SORT_CMC);
    ASSERT(cards.current_sort == MSE_SORT_CMC);
    ASSERT(mse_avl_cmp_card_cmc(cards.cards[0], cards.cards[1]) < 0);

    free(cards.cards);
    return 1;
}

static int test_sort_card_uuid()
{
    // Setup mock cards
    mse_card_t card_a, card_b;
    memset(&card_a, 0, sizeof(card_a));
    card_b = card_a;

    memset(&card_a.id.bytes, 1, sizeof(card_a.id.bytes));
    memset(&card_b.id.bytes, 2, sizeof(card_b.id.bytes));

    mse_search_result_t cards;
    memset(&cards, 0, sizeof(cards));
    cards.current_sort = MSE_SORT_END;

    cards.cards_length = 2;
    ASSERT(cards.cards = malloc(sizeof(cards.cards) * cards.cards_length));

    cards.cards[0] = &card_a;
    cards.cards[1] = &card_b;

    // Sort
    mse_sort_search_results(&cards, MSE_SORT_UUID);
    ASSERT(cards.current_sort == MSE_SORT_UUID);
    ASSERT(mse_avl_cmp_card(cards.cards[0], cards.cards[1]) < 0);

    free(cards.cards);
    return 1;
}

static int test_sort_card_power()
{
    // Setup mock cards
    mse_card_t card_a, card_b;
    memset(&card_a, 0, sizeof(card_a));
    card_b = card_a;

    card_a.power = 1;
    memset(&card_a.id.bytes, 1, sizeof(card_a.id.bytes));

    card_b.power= 123;
    memset(&card_b.id.bytes, 2, sizeof(card_b.id.bytes));

    mse_search_result_t cards;
    memset(&cards, 0, sizeof(cards));
    cards.current_sort = MSE_SORT_END;

    cards.cards_length = 2;
    ASSERT(cards.cards = malloc(sizeof(cards.cards) * cards.cards_length));

    cards.cards[0] = &card_a;
    cards.cards[1] = &card_b;

    // Sort
    mse_sort_search_results(&cards, MSE_SORT_POWER);
    ASSERT(cards.current_sort == MSE_SORT_POWER);
    ASSERT(mse_avl_cmp_card_power(cards.cards[0], cards.cards[1]) < 0);

    free(cards.cards);
    return 1;
}

static int test_sort_card_toughness()
{
    // Setup mock cards
    mse_card_t card_a, card_b;
    memset(&card_a, 0, sizeof(card_a));
    card_b = card_a;

    card_a.toughness = 1;
    memset(&card_a.id.bytes, 1, sizeof(card_a.id.bytes));

    card_b.toughness = 123;
    memset(&card_b.id.bytes, 2, sizeof(card_b.id.bytes));

    mse_search_result_t cards;
    memset(&cards, 0, sizeof(cards));
    cards.current_sort = MSE_SORT_END;

    cards.cards_length = 2;
    ASSERT(cards.cards = malloc(sizeof(cards.cards) * cards.cards_length));

    cards.cards[0] = &card_a;
    cards.cards[1] = &card_b;

    // Sort
    mse_sort_search_results(&cards, MSE_SORT_TOUGHNESS);
    ASSERT(cards.current_sort == MSE_SORT_TOUGHNESS);
    ASSERT(mse_avl_cmp_card_toughness(cards.cards[0], cards.cards[1]) < 0);

    free(cards.cards);
    return 1;
}

// The finalise search and the search results are tested in the interpretor where there is a lot of test data

SUB_TEST(test_search, {&test_union_tree, "Test set union on trees"},
{&test_intersection_tree, "Test set intersection on trees"},
{&test_set_negation, "Test set negation"},
{&test_sort_same_as_current_sort, "Test sort as same as current sort"},
{&test_sort_card_name, "Test sort by card name"},
{&test_sort_card_uuid, "Test sort by card UUID"},
{&test_sort_card_power, "Test sort by card power"},
{&test_sort_card_toughness, "Test sort by card toughness"},
{&test_sort_card_cmc, "Test sory by card cmc"})
