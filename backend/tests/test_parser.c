#include "../testing_h/testing.h"
#include "../mse/query_parser.h"
#include "../mse/interpretor.h"
#include "../mse/save.h"
#include "./test_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static mse_thread_pool_t pool;
static mse_all_printings_cards_t cards;

static int __test_next_query(FILE *f)
{
    char buffer[1024];
    char *r = fgets(buffer, sizeof(buffer), f);

    // EOF
    if (r == NULL) {
        return EOF;
    }
    // Empty string is also EOF
    if (buffer[0] == 0) {
        return EOF;
    }

    // Strip the last char (\n)
    if (strlen(buffer) < 1) return 1;
    size_t last = strlen(buffer) - 1;
    if (buffer[last] == '\n') {
        buffer[last] = 0;
    }

    lprintf(TEST_INFO, "Testing parse of '%s'\n", buffer);
    mse_interp_node_t *ret = NULL;
    ASSERT(mse_parse_input_string(buffer, &ret));
    ASSERT(ret != NULL);

    mse_free_interp_node(ret);

    lprintf(TEST_PASS, "Passed\n");
    return 1;
}

static int __test_next_query_exec(FILE *f)
{
    char buffer[1024];
    char *r = fgets(buffer, sizeof(buffer), f);

    // EOF
    if (r == NULL) {
        return EOF;
    }
    // Empty string is also EOF
    if (buffer[0] == 0) {
        return EOF;
    }

    // Strip the last char (\n)
    if (strlen(buffer) < 1) return 1;
    size_t last = strlen(buffer) - 1;
    if (buffer[last] == '\n') {
        buffer[last] = 0;
    }

    lprintf(TEST_INFO, "Testing parse of '%s'\n", buffer);
    mse_interp_node_t *ret = NULL;
    ASSERT(mse_parse_input_string(buffer, &ret));

    ASSERT(ret != NULL);

    mse_search_intermediate_t s_ret;
    ASSERT(mse_resolve_interp_tree(ret, &s_ret, &pool, 0, &cards));
    lprintf(LOG_INFO, "There were %lu results\n", mse_tree_size(s_ret.node));
    mse_free_search_intermediate(&s_ret);
    mse_free_interp_node(ret);

    lprintf(TEST_PASS, "Passed\n");
    return 1;
}

static int init_parser_tests()
{
    ASSERT(mse_init_pool(&pool));
    ASSERT(mse_get_cards_from_file(&cards, &pool));
    ASSERT(cards.card_tree != NULL);
    return 1;
}

static int free_parser_tests()
{
    mse_free_all_printings_cards(&cards);
    ASSERT(mse_free_pool(&pool));
    return 1;
}

static int __test_parser()
{
    FILE *f = fopen("./valid_queries.txt", "r");
    ASSERT(f != NULL);

    int s = 1;
    int flag = 1;
    do {
        flag = __test_next_query(f);
        if (!flag) {
            s = 0;
        }
    } while (flag != EOF);

    fclose(f);
    return s;
}

static int test_parse_and_exec()
{
    FILE *f = fopen("./valid_queries.txt", "r");
    ASSERT(f != NULL);

    int s = 1;
    int flag = 1;
    do {
        flag = __test_next_query_exec(f);
        if (!flag) {
            s = 0;
        }
    } while (flag != EOF);

    fclose(f);
    return s;
}

static int __test_next_query_fail_case(FILE *f)
{
    char buffer[1024];
    char *r = fgets(buffer, sizeof(buffer), f);

    // EOF
    if (r == NULL) {
        return EOF;
    }
    // Empty string is also EOF
    if (buffer[0] == 0) {
        return EOF;
    }

    // Strip the last char (\n)
    if (strlen(buffer) < 1) return 1;
    size_t last = strlen(buffer) - 1;
    if (buffer[last] == '\n') {
        buffer[last] = 0;
    }

    lprintf(TEST_INFO, "Testing parse of '%s' fails\n", buffer);
    mse_interp_node_t *ret = NULL;
    ASSERT(!mse_parse_input_string(buffer, &ret));
    mse_free_interp_node(ret);

    lprintf(TEST_PASS, "Passed\n");
    return 1;
}

static int test_parse_invalid_queries()
{
    FILE *f = fopen("./invalid_queries.txt", "r");
    ASSERT(f != NULL);

    int s = 1;
    int flag = 1;
    do {
        flag = __test_next_query_fail_case(f);
        if (!flag) {
            s = 0;
        }
    } while (flag != EOF);

    fclose(f);
    return s;
}

SUB_TEST(test_parser, {&init_parser_tests, "Init parser tests"},
{&__test_parser, "Run parser tests"},
{&test_parse_and_exec, "Parse then execute"},
{&test_parse_invalid_queries, "Run parser fail case tests"},
{&free_parser_tests, "Free parser tests"})
