#include "./test_card_txt_field_trie.h"
#include "../testing_h/testing.h"
#include "../mse/card_txt_fields_trie.h"
#include "../mse/uuid.h"
#include <stdlib.h>
#include <string.h>

#define DANDAN "Dandân"
#define DANDAN_F "dndn"

#define DUPLICATE_TEXT "aaaaaaaabbbbbbbbbccccc123"
#define DUPLICATE_TEXT_F "bc"

#define EMPTY_TEXT "      /  /  / /  /  /      / /  /  /  /  /  / /  /,    /, .  // / , ., /"

static int test_filter_str()
{
    char *dandan = mse_filter_text(DANDAN);
    lprintf(LOG_INFO, "%s\n", dandan);
    ASSERT(dandan != NULL);
    ASSERT(strcmp(dandan, DANDAN_F) == 0);
    free(dandan);

    char *dup_text = mse_filter_text(DUPLICATE_TEXT);
    ASSERT(dup_text != NULL);
    ASSERT(strcmp(dup_text, DUPLICATE_TEXT_F) == 0);
    free(dup_text);

    char *empty_str = mse_filter_text(EMPTY_TEXT);
    ASSERT(empty_str == NULL);
    return 1;
}

static int test_trie_init_free()
{
    mse_card_trie_node_t *node = NULL;
    ASSERT(init_mse_card_trie_node(&node));
    ASSERT(node != NULL);

    for (size_t i = 0; i < sizeof(node->children) / sizeof(*node->children); i++) {
        ASSERT(node->children[i] == NULL);
    }
    free_mse_card_trie_node(node);
    return 1;
}

static int test_trie_init_free_children()
{
    mse_card_trie_node_t *node = NULL;
    ASSERT(init_mse_card_trie_node(&node));
    ASSERT(node != NULL);

    mse_card_trie_node_t *node_2 = NULL;
    ASSERT(init_mse_card_trie_node(&node_2));
    ASSERT(node_2 != NULL);
    node->children[3] = node_2;

    free_mse_card_trie_node(node);
    return 1;
}

#define GET_INDEX(str, i) (str[i] - 'a')

static int test_card_insert()
{
    mse_card_trie_node_t *node = NULL;
    ASSERT(init_mse_card_trie_node(&node));
    ASSERT(node != NULL);

    mse_card_t card;
    memset(&card, 0, sizeof(card));
    ASSERT(mse_card_trie_insert(node, &card, DANDAN));

    mse_card_trie_node_t *root = node;
    for (size_t i = 0; i < strlen(DANDAN_F); i++) {
        root = root->children[GET_INDEX(DANDAN_F, i)];
        ASSERT(root != NULL);
    }

    free_mse_card_trie_node(node);
    return 1;
}

static int test_card_lookup()
{
    mse_card_trie_node_t *node = NULL;
    ASSERT(init_mse_card_trie_node(&node));
    ASSERT(node != NULL);

    mse_card_t card;
    memset(&card, 0, sizeof(card));
    ASSERT(mse_card_trie_insert(node, &card, DANDAN));

    avl_tree_node_t *ret = NULL;
    ASSERT(mse_card_trie_lookup(node, DANDAN, &ret));
    ASSERT(ret != NULL);
    free_tree(ret);

    ASSERT(mse_card_trie_lookup(node, DANDAN_F, &ret));
    ASSERT(ret != NULL);
    free_tree(ret);

    ASSERT(mse_card_trie_lookup(node, "poopoo", &ret));
    ASSERT(ret == NULL);

    free_mse_card_trie_node(node);
    return 1;
}

static int test_card_lookup_aprox()
{
    mse_card_trie_node_t *node = NULL;
    ASSERT(init_mse_card_trie_node(&node));
    ASSERT(node != NULL);

    // This test case is cursed I am so sorry
    mse_card_t dandan;
    memset(&dandan, 0, sizeof(dandan));
    ASSERT(mse_card_trie_insert(node, &dandan, DANDAN));

    mse_card_t dandanier;
    memset(&dandanier, 0, sizeof(dandanier));
    dandanier.id = max_uuid();
    ASSERT(mse_card_trie_insert(node, &dandanier, DANDAN "ier"));

    mse_card_t dandanierier;
    memset(&dandanierier, 0, sizeof(dandanierier));
    dandanierier.id.bytes[0] = 3;
    ASSERT(mse_card_trie_insert(node, &dandanierier, DANDAN "ierier"));

    avl_tree_node_t *ret = NULL;
    ASSERT(mse_card_trie_lookup_aprox(node, DANDAN, &ret));
    ASSERT(ret != NULL);
    ASSERT(tree_size(ret) == 3);
    free_tree(ret);

    ASSERT(mse_card_trie_lookup_aprox(node, "hasuidhasijdh" DANDAN, &ret));
    ASSERT(ret == NULL);

    free_mse_card_trie_node(node);
    return 1;
}

#define SPLIT_STR "Goblin   , Motivator      "
#define PARTS 2

static int test_card_name_split()
{
    mse_card_name_parts_t ret;
    ASSERT(mse_split_card_name(SPLIT_STR, &ret));
    ASSERT(ret.parts != NULL);
    ASSERT(ret.len == PARTS);

    for (size_t i = 0; i < ret.len; i++) {
        ASSERT(ret.parts[i] != NULL);
        ASSERT(strlen(ret.parts[i]) > 0);

        char *fname = mse_filter_text(ret.parts[i]);
        ASSERT(strcmp(fname, ret.parts[i]) == 0);
        free(fname);
    }

    ASSERT(strcmp(ret.parts[0], "gbln") == 0);
    ASSERT(strcmp(ret.parts[1], "mtvtr") == 0);

    free_mse_card_parts(&ret);
    return 1;
}

SUB_TEST(test_card_txt_field_trie, {&test_filter_str, "Test filter string"},
{&test_trie_init_free, "Test trie init and free"},
{&test_trie_init_free_children, "Test trie init and free with a child"},
{&test_card_insert, "Test trie card insert"},
{&test_card_lookup, "Test trie card lookup"},
{&test_card_lookup_aprox, "Test trie card lookup aprox"},
{&test_card_name_split, "Test card name split"})
