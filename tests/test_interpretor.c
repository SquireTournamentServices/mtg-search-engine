#include "./test_interpretor.h"
#include "../testing_h/testing.h"
#include "../mse/interpretor.h"
#include <string.h>

// Some vile testing globals
static mse_all_printings_cards_t test_cards;
static mse_thread_pool_t pool;

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
    ASSERT(mse_init_pool(&pool));

    json_t *json = get_all_printings_cards_from_file();
    ASSERT(json != NULL);

    memset(&test_cards, 0, sizeof(test_cards));
    ASSERT(__mse_parse_all_printings_cards(&test_cards, json, &pool));
    json_decref(json);

    ASSERT(test_cards.card_tree != NULL);

    return 1;
}

static int free_test_card()
{
    mse_free_all_printings_cards(&test_cards);
    ASSERT(mse_free_pool(&pool));
    return 1;
}

static int __test_init_free_operator(mse_set_operator_type_t op_type)
{
    mse_interp_node_t *node = NULL;
    ASSERT(node = mse_init_interp_node_operation(op_type));
    ASSERT(node->type == MSE_INTERP_NODE_SET_OPERATOR);
    ASSERT(node->op_type == op_type);
    ASSERT(node->l == NULL);
    ASSERT(node->r == NULL);

    mse_free_interp_node(node);
    return 1;
}

static int test_init_free_operator()
{
    ASSERT(__test_init_free_operator(MSE_SET_UNION));
    ASSERT(__test_init_free_operator(MSE_SET_INTERSECTION));
    return 1;
}

static int test_init_free_generator()
{
    mse_set_generator_t generator;
    memset(&generator, 0x7F, sizeof(generator));

    mse_interp_node_t *node = NULL;
    ASSERT(node = mse_init_interp_node_generator(generator));
    ASSERT(node->type == MSE_INTERP_NODE_SET_GENERATOR);
    ASSERT(memcmp(&node->generator, &generator, sizeof(generator)) == 0);
    ASSERT(node->l == NULL);
    ASSERT(node->r == NULL);

    // Make the generator NULL so that it doesn't frick up
    memset(&node->generator, 0, sizeof(node->generator));
    mse_free_interp_node(node);
    return 1;
}

static int test_recursive_free()
{
    mse_interp_node_t *node = NULL;
    ASSERT(node = mse_init_interp_node_operation(MSE_SET_UNION));

    mse_set_generator_t generator;
    memset(&generator, 0, sizeof(generator));

    ASSERT(node->l = mse_init_interp_node_operation(MSE_SET_UNION));
    ASSERT(node->l->l = mse_init_interp_node_generator(generator));
    ASSERT(node->l->r = mse_init_interp_node_generator(generator));

    ASSERT(node->r = mse_init_interp_node_operation(MSE_SET_UNION));
    ASSERT(node->r->l = mse_init_interp_node_generator(generator));
    ASSERT(node->r->r = mse_init_interp_node_generator(generator));

    mse_free_interp_node(node);
    return 1;
}

#define ARG "M20"

static int test_resolve_set_generator()
{
    mse_set_generator_t generator;
    ASSERT(mse_init_set_generator(&generator, MSE_SET_GENERATOR_SET, MSE_SET_GENERATOR_OP_EQUALS, ARG, strlen(ARG)));

    mse_interp_node_t *node;
    ASSERT(node = mse_init_interp_node_generator(generator));

    mse_search_intermediate_t ret;
    ASSERT(mse_resolve_interp_tree(node, &ret, &pool, 0, &test_cards));
    ASSERT(ret.node != NULL);
    ASSERT(tree_size(ret.node) >= 329);
    mse_free_search_intermediate(&ret);

    ret.node = NULL;
    ASSERT(mse_resolve_interp_tree(node, &ret, &pool, 1, &test_cards));
    ASSERT(ret.node == NULL);

    // Cleanup
    mse_free_interp_node(node);
    return 1;
}

#define REGEX_1 "/.*2.*/"
#define COLOUR_1 "r"

static int test_resolve_tree_1()
{
    mse_interp_node_t *root;
    ASSERT(root = mse_init_interp_node_operation(MSE_SET_INTERSECTION));

    mse_set_generator_t generator;
    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_ORACLE_TEXT,
                                  MSE_SET_GENERATOR_OP_EQUALS,
                                  REGEX_1,
                                  strlen(REGEX_1)));
    ASSERT(root->l = mse_init_interp_node_generator(generator));

    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_COLOUR,
                                  MSE_SET_GENERATOR_OP_GT_INC,
                                  COLOUR_1,
                                  strlen(COLOUR_1)));
    ASSERT(root->r = mse_init_interp_node_generator(generator));

    mse_search_intermediate_t ret;
    ASSERT(mse_resolve_interp_tree(root, &ret, &pool, 0, &test_cards));
    ASSERT(ret.node != NULL);
    ASSERT(tree_size(ret.node) >= 6);
    mse_free_search_intermediate(&ret);

    ret.node = NULL;
    ASSERT(mse_resolve_interp_tree(root, &ret, &pool, 1, &test_cards));
    ASSERT(ret.node== NULL);

    // Cleanup
    mse_free_interp_node(root);
    return 1;
}

#define COLOUR_2 "g"
#define COLOUR_3 "wubrg"

// (colour>=r and colour>=g) and colour:wubrg
static int test_resolve_tree_2()
{
    mse_interp_node_t *root;
    ASSERT(root = mse_init_interp_node_operation(MSE_SET_INTERSECTION));

    ASSERT(root->l = mse_init_interp_node_operation(MSE_SET_INTERSECTION));

    mse_set_generator_t generator;
    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_COLOUR,
                                  MSE_SET_GENERATOR_OP_GT_INC,
                                  COLOUR_1,
                                  strlen(COLOUR_1)));
    ASSERT(root->r = mse_init_interp_node_generator(generator));

    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_COLOUR,
                                  MSE_SET_GENERATOR_OP_GT_INC,
                                  COLOUR_2,
                                  strlen(COLOUR_2)));
    ASSERT(root->l->r = mse_init_interp_node_generator(generator));

    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_COLOUR,
                                  MSE_SET_GENERATOR_OP_GT_INC,
                                  COLOUR_3,
                                  strlen(COLOUR_3)));
    ASSERT(root->l->l = mse_init_interp_node_generator(generator));

    mse_search_intermediate_t ret;
    ASSERT(mse_resolve_interp_tree(root, &ret, &pool, 0, &test_cards));
    ASSERT(ret.node != NULL);
    ASSERT(tree_size(ret.node) >= 45);
    mse_free_search_intermediate(&ret);

    ret.node = NULL;
    ASSERT(mse_resolve_interp_tree(root, &ret, &pool, 1, &test_cards));
    ASSERT(ret.node== NULL);

    // Cleanup
    mse_free_interp_node(root);
    return 1;
}

static int test_resolve_tree_3()
{
    mse_interp_node_t *root;
    ASSERT(root = mse_init_interp_node_operation(MSE_SET_INTERSECTION));
    ASSERT(root->l = mse_init_interp_node_operation(MSE_SET_INTERSECTION));
    ASSERT(root->r = mse_init_interp_node_operation(MSE_SET_INTERSECTION));

    mse_set_generator_t generator;
    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_ORACLE_TEXT,
                                  MSE_SET_GENERATOR_OP_EQUALS,
                                  REGEX_1,
                                  strlen(REGEX_1)));
    ASSERT(root->l->r = mse_init_interp_node_generator(generator));

    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_ORACLE_TEXT,
                                  MSE_SET_GENERATOR_OP_EQUALS,
                                  REGEX_1,
                                  strlen(REGEX_1)));
    ASSERT(root->l->l = mse_init_interp_node_generator(generator));

    // Right
    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_ORACLE_TEXT,
                                  MSE_SET_GENERATOR_OP_EQUALS,
                                  REGEX_1,
                                  strlen(REGEX_1)));
    ASSERT(root->r->r = mse_init_interp_node_generator(generator));

    ASSERT(mse_init_set_generator(&generator,
                                  MSE_SET_GENERATOR_ORACLE_TEXT,
                                  MSE_SET_GENERATOR_OP_EQUALS,
                                  REGEX_1,
                                  strlen(REGEX_1)));
    ASSERT(root->r->l = mse_init_interp_node_generator(generator));

    mse_search_intermediate_t ret;
    ASSERT(mse_resolve_interp_tree(root, &ret, &pool, 0, &test_cards));
    ASSERT(ret.node != NULL);
    mse_free_search_intermediate(&ret);

    ret.node = NULL;
    ASSERT(mse_resolve_interp_tree(root, &ret, &pool, 1, &test_cards));
    ASSERT(ret.node== NULL);

    // Cleanup
    mse_free_interp_node(root);
    return 1;
}

SUB_TEST(test_interpretor, {&test_init_free_operator, "Test init free for operator node"},
{&test_init_free_generator, "Test init free for generator node"},
{&test_recursive_free, "Test recursive free"},
{&init_test_cards, "Init test cards"},
{&test_resolve_set_generator, "Test resolve generator"},
{&test_resolve_tree_1, "Test resolve tree 1"},
{&test_resolve_tree_2, "Test resolve tree 2"},
{&test_resolve_tree_3, "Test resolve tree 3"},
{&free_test_card, "Free test cards"})
