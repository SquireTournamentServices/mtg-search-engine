#include "./test_generators.h"
#include "../src/generators.h"
#include "../src/card_str_match.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <regex.h>

#define REGEX_ARG "/.*god of (the )?.*/"

static int test_tree_oracle_re(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    regex_t re;
    ASSERT(mse_compile_regex(REGEX_ARG, &re));

    mtg_card_t *card = (mtg_card_t *) node->payload;
    int found = 0;
    for (size_t i = 0; i < card->set_codes_count; i++) {
        ASSERT(mse_card_oracle_matches(card, &re));
    }
    regfree(&re);
    ASSERT(found);

    ASSERT(test_tree_oracle_re(node->l));
    ASSERT(test_tree_oracle_re(node->r));
    return 1;
}

static int test_generator_oracle_regex()
{
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_ORACLE_TEXT;
    size_t len = strlen(REGEX_ARG);

    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_EQUALS, REGEX_ARG, len));

    avl_tree_node_t *root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree_oracle_re(root));
    free_tree(root);
    mse_free_set_generator(&ret);

    // Test includes
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_INCLUDES, REGEX_ARG, len));
    root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree_oracle_re(root));
    free_tree(root);
    mse_free_set_generator(&ret);
    return 1;
}

static int test_tree_name_re(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    regex_t re;
    ASSERT(mse_compile_regex(REGEX_ARG, &re));

    mtg_card_t *card = (mtg_card_t *) node->payload;
    int found = 0;
    for (size_t i = 0; i < card->set_codes_count; i++) {
        ASSERT(mse_card_name_matches(card, &re));
    }
    regfree(&re);
    ASSERT(found);

    ASSERT(test_tree_name_re(node->l));
    ASSERT(test_tree_name_re(node->r));
    return 1;
}

static int test_generator_name_regex()
{
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_NAME;
    size_t len = strlen(REGEX_ARG);

    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_EQUALS, REGEX_ARG, len));

    avl_tree_node_t *root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree_name_re(root));
    free_tree(root);
    mse_free_set_generator(&ret);

    // Test includes
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_INCLUDES, REGEX_ARG, len));
    root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree_name_re(root));
    free_tree(root);
    mse_free_set_generator(&ret);
    return 1;
}

SUB_TEST(test_generator_txt, {&test_generator_oracle_regex, "Test generator oracle"},
{&test_generator_name_regex, "Test generator name"})
