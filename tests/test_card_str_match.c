#include "./test_card_str_match.h"
#include "../testing_h/testing.h"
#include "../src/card_str_match.h"
#include "../src/mtg_json.h"
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <regex.h>

// Some vile testing globals
static mtg_all_printings_cards_t test_cards;
static thread_pool_t pool;

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

static int init_test_cards()
{
    ASSERT(init_pool(&pool));

    json_t *json = get_all_printings_cards_from_file();
    ASSERT(json != NULL);

    memset(&test_cards, 0, sizeof(test_cards));
    ASSERT(__parse_all_printings_cards(&test_cards, json, &pool));
    json_decref(json);

    ASSERT(test_cards.card_tree != NULL);

    return 1;
}

static int free_test_card()
{
    free_all_printings_cards(&test_cards);
    ASSERT(free_pool(&pool));
    return 1;
}

#define ORACLE_TEST_REGEX_1_MATCHES 58
#define ORACLE_TEST_REGEX_1 ".*whenever a (creature|enchantment) enters the battlefield.*"
#define ORACLE_TEST_REGEX_2_MATHCES 2
#define ORACLE_TEST_REGEX_2 "whenever .* enters the battlefield,.*draw (a|[0-9]+) cards?.*"

static int test_card_matches()
{
    regex_t re;
    ASSERT(mse_compile_regex(ORACLE_TEST_REGEX_1, &re));

    mtg_card_t card;
    card.name = "Testing name 123";
    card.oracle_text = "Whenever a creature enters the battlefield, pass go and collect $200.";

    for (size_t i = 0; i < 100; i++) {
        ASSERT(mse_card_oracle_matches(&card, &re));
        ASSERT(!mse_card_name_matches(&card, &re));
    }
    regfree(&re);
    return 1;
}

static int test_oracle_match()
{
    avl_tree_node_t *ret = NULL;
    ASSERT(mse_matching_card_oracle(&ret, test_cards.card_tree, ORACLE_TEST_REGEX_1, &pool));
    ASSERT(ret != NULL);
    ASSERT(tree_size(ret) >= ORACLE_TEST_REGEX_1_MATCHES);
    free_tree(ret);
    return 1;
}

static int test_oracle_match_2()
{
    avl_tree_node_t *ret = NULL;
    ASSERT(mse_matching_card_oracle(&ret, test_cards.card_tree, ORACLE_TEST_REGEX_2, &pool));
    ASSERT(ret != NULL);
    ASSERT(tree_size(ret) >= ORACLE_TEST_REGEX_2_MATHCES);
    free_tree(ret);
    return 1;
}

#define NAME_TEST_REGEX_1_MATCHES 29
#define NAME_TEST_REGEX_1 ".*, god of .*"
#define NAME_TEST_REGEX_2_MATHCES 17
#define NAME_TEST_REGEX_2 ".* class$"

static int test_name_match()
{
    avl_tree_node_t *ret = NULL;
    ASSERT(mse_matching_card_name(&ret, test_cards.card_tree, NAME_TEST_REGEX_1, &pool));
    ASSERT(ret != NULL);
    ASSERT(tree_size(ret) >= NAME_TEST_REGEX_1_MATCHES);
    free_tree(ret);
    return 1;
}

static int test_name_match_2()
{
    avl_tree_node_t *ret = NULL;
    ASSERT(mse_matching_card_name(&ret, test_cards.card_tree, NAME_TEST_REGEX_2, &pool));
    ASSERT(ret != NULL);
    ASSERT(tree_size(ret) >= NAME_TEST_REGEX_2_MATHCES);
    free_tree(ret);
    return 1;
}

#define INVALID_RE "("

static int test_regex_compile_err()
{
    regex_t re;
    ASSERT(mse_compile_regex(INVALID_RE, &re) == 0);

    avl_tree_node_t *ret = NULL;
    ASSERT(!mse_matching_card_oracle(&ret, test_cards.card_tree, INVALID_RE, &pool));
    ASSERT(ret == NULL);

    ASSERT(!mse_matching_card_name(&ret, test_cards.card_tree, INVALID_RE, &pool));
    ASSERT(ret == NULL);
    return 1;
}

static int test_oracle_match_a_lot_of_times()
{
    // On my machine it took 50 seconds for 10,000 this is probably fine
    for (size_t i = 0; i < 100; i++) {
        ASSERT(test_name_match());
    }
    return 1;
}

#define NO_REPLACEMENT_STR "abcdefgh"
#define STRIPPED_STR "test132123123(abc)+"
#define STRIP_STR "/" STRIPPED_STR "/"

#define ESCAPED_SLASH "/1/1/"

static int test_regex_escape()
{
    char *tmp = escape_regex(NO_REPLACEMENT_STR);
    ASSERT(tmp != NULL);
    ASSERT(strcmp(tmp, NO_REPLACEMENT_STR) == 0);
    free(tmp);

    tmp = escape_regex(STRIP_STR);
    ASSERT(tmp != NULL);
    ASSERT(strcmp(tmp, STRIPPED_STR) == 0);
    free(tmp);

    tmp = escape_regex(ESCAPED_SLASH);
    ASSERT(tmp != NULL);
    ASSERT(strcmp("1/1", tmp) == 0);
    free(tmp);
    return 1;
}

SUB_TEST(test_card_str_match, {&init_test_cards, "Init regex test cards"},
{&test_card_matches, "Test card matches"},
{&test_oracle_match, "Test oracle regex match"},
{&test_oracle_match_2, "Test oracle regex match 2"},
{&test_name_match, "Test name regex match"},
{&test_name_match_2, "Test name regex match 2"},
{&test_regex_compile_err, "Test regex compile error case"},
{&test_oracle_match_a_lot_of_times, "Test oracle match a lot of times"},
{&test_regex_escape, "Test regex escape"},
{&free_test_card, "Free regex test cards"})
