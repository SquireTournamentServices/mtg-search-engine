#include "./test_generators.h"
#include "../testing_h/testing.h"
#include "../src/generators.h"
#include <string.h>
#include <jansson.h>

#define DEFAULT_ARGUMENT "thopt"

thread_pool_t gen_thread_pool;
mse_all_printings_cards_t gen_cards;

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

static int init_generator_tests()
{
    ASSERT(init_pool(&gen_thread_pool));

    json_t *json = get_all_printings_cards_from_file();
    ASSERT(json != NULL);

    memset(&gen_cards, 0, sizeof(gen_cards));
    ASSERT(__parse_all_printings_cards(&gen_cards, json, &gen_thread_pool));
    json_decref(json);

    ASSERT(gen_cards.card_tree != NULL);

    return 1;
}

static int free_generator_tests()
{
    free_all_printings_cards(&gen_cards);
    ASSERT(free_pool(&gen_thread_pool));
    return 1;
}

static int test_generator_init_free()
{
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_COLOUR;
    mse_set_generator_operator_t op_type = MSE_SET_GENERATOR_OP_LT_INC;
    size_t len = strlen(DEFAULT_ARGUMENT);

    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));

    ASSERT(ret.argument != NULL);
    ASSERT(strcmp(ret.argument, DEFAULT_ARGUMENT) == 0);
    ASSERT(ret.generator_type == gen_type);
    ASSERT(ret.generator_op == op_type);

    mse_free_set_generator(&ret);

    // Test NULL generator
    memset(&ret, 0, sizeof(ret));
    mse_free_set_generator(&ret);
    return 1;
}

SUB_TEST(test_generators, {&init_generator_tests, "Init generator tests"},
{&test_generator_init_free, "Test generator init free"},
{&test_generator_validation, "Test generator validation"},
{&test_set_generators_double_fields, "Test set generators double fields"},
{&test_generator_set, "Test set generators"},
{&test_generator_txt, "Test txt generators (name & oracle)"},
{&test_generator_colours, "Test colours generators"},
{&free_generator_tests, "Free generator tests"})
