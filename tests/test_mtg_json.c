#include "./test_mtg_json.h"
#include "../testing_h/testing.h"
#include "../src/mtg_json.h"
#include "../src/thread_pool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>

// Some vile testing globals
static mtg_all_printings_cards_t test_cards;
static json_t *json;


static size_t get_tree_nodes(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 0;
    }
    return 1 + get_tree_nodes(node->l) + get_tree_nodes(node->r);
}


// A test for the tree props
static int test_all_printings_cards_sets_found()
{
    ASSERT(test_cards.set_tree != NULL);
    ASSERT(test_cards.set_tree->height > 0);
    ASSERT(get_tree_nodes(test_cards.set_tree) == test_cards.set_count);
    return 1;
}

static int test_all_printings_cards_found()
{
    ASSERT(test_cards.card_tree != NULL);
    ASSERT(test_cards.card_tree->height > 0);
    ASSERT(get_tree_nodes(test_cards.card_tree) == test_cards.card_count);
    return 1;
}

SUB_TEST(__test_atomic_card_props, {&test_all_printings_cards_sets_found, "Test atomic cards found sets"},
{&test_all_printings_cards_found, "Test atomic cards found cards"})

// Start tests
static int test_free_all_printings_cards()
{
    mtg_all_printings_cards_t ret;
    memset(&ret, 0, sizeof(ret));
    free_all_printings_cards(&ret);
    return 1;
}

static int test_init_free()
{
    thread_pool_t pool;
    ASSERT(init_pool(&pool));

    memset(&test_cards, 0, sizeof(test_cards));
    ASSERT(get_all_printings_cards(&test_cards, &pool));

    ASSERT(test_cards.indexes.card_p_tree != NULL);
    size_t p_nodes = get_tree_nodes(test_cards.indexes.card_p_tree);
    ASSERT(p_nodes > 0);

    ASSERT(test_cards.indexes.card_t_tree != NULL);
    size_t t_nodes = get_tree_nodes(test_cards.indexes.card_t_tree);
    ASSERT(t_nodes > 0);
    ASSERT(p_nodes == t_nodes);
    ASSERT(p_nodes == test_cards.card_count);

    ASSERT(__test_atomic_card_props());
    free_all_printings_cards(&test_cards);

    ASSERT(free_pool(&pool));
    return 1;
}

#define TEST_TEXT "Lorem ipsum dolor sit amet, qui minim labore adipisicing minim sint cillum sint consectetur cupidatat."

static int test_curl_write_callback()
{
    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb");
    ASSERT(r != NULL);

    FILE *w = fdopen(fid[1], "wb");
    ASSERT(w != NULL);

    size_t len = sizeof(TEST_TEXT);
    ASSERT(__mtg_json_write_callback(TEST_TEXT, len, 1, (void *) w) == len);
    fclose(w);

    char buffer[1024];
    ASSERT(fgets(buffer, sizeof(buffer), r) != NULL);
    fclose(r);

    ASSERT(strcmp(buffer, TEST_TEXT) == 0);
    return 1;
}

static json_t *get_all_printings_cards_from_file()
{
    FILE *f = fopen("./AllPrintings.json", "rb");
    json_error_t error;
    json_t *ret = json_loadf(f, 0, &error);

    if (ret == NULL) {
        lprintf(LOG_ERROR, "Error: %100s\n", error.text);
    }
    fclose(f);
    return ret;
}

static int test_parse_all_printings_cards_sets()
{
    json = get_all_printings_cards_from_file();
    ASSERT(json != NULL);

    memset(&test_cards, 0, sizeof(test_cards));
    ASSERT(__parse_all_printings_cards(&test_cards, json));
    ASSERT(__test_atomic_card_props());

    free_all_printings_cards(&test_cards);
    json_decref(json);
    return 1;
}

SUB_TEST(test_mtg_json, {&test_free_all_printings_cards, "Test free zeroed atomic cards struct"},
{&test_init_free, "Test init and, free"},
{&test_curl_write_callback, "Test cURL write callback"},
{&test_parse_all_printings_cards_sets, "Test that __parse_all_printings_cards reads the sets correctly"})
