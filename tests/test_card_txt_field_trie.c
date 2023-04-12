#include "./test_card_txt_field_trie.h"
#include "../testing_h/testing.h"
#include "../src/card_txt_fields_trie.h"
#include <stdlib.h>
#include <string.h>

#define DANDAN "Dandân"
#define DANDAN_F "dndn"

#define DUPLICATE_TEXT "aaaaaaaabbbbbbbbbccccc"
#define DUPLICATE_TEXT_F "bc"

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
    return 1;
}

static int test_trie_init_free()
{
    mse_card_trie_node_t *node = NULL;
    ASSERT(init_mse_card_trie_node(&node));
    ASSERT(node != NULL);
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

SUB_TEST(test_card_txt_field_trie, {&test_filter_str, "Test filter string"},
{&test_trie_init_free, "Test trie init and free"},
{&test_trie_init_free_children, "Test trie init and free with a child"})
