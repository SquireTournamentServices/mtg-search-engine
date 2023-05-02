#include "./test_generators.h"
#include "../src/card.h"
#include "../src/generators.h"
#include "../src/avl_tree.h"
#include "../testing_h/testing.h"
#include <string.h>

#define DEFAULT_ARGUMENT (MSE_WHITE | MSE_BLUE)
#define DEFAULT_ARGUMENT_STR "WU"

#define COLOUR_GEN_TEST(op_l, op_u, op) \
static int test_tree_c_##op_l(avl_tree_node_t *node) \
{ \
    if (node == NULL) { \
        return 1; \
    } \
 \
    mse_card_t *card = (mse_card_t *) node->payload; \
    ASSERT(mse_colour_##op(card->colours, DEFAULT_ARGUMENT)); \
 \
    ASSERT(test_tree_c_##op_l(node->l)); \
    ASSERT(test_tree_c_##op_l(node->r)); \
    return 1; \
} \
 \
static int test_generator_colours_##op_l() \
{ \
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_COLOUR; \
    size_t len = strlen(DEFAULT_ARGUMENT_STR); \
 \
    mse_set_generator_t ret; \
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_##op_u, DEFAULT_ARGUMENT_STR, len)); \
 \
    avl_tree_node_t *root = NULL; \
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool)); \
    ASSERT(tree_size(root) > 0); \
    ASSERT(test_tree_c_##op_l(root)); \
 \
    free_tree(root); \
    mse_free_set_generator(&ret); \
    return 1; \
}

COLOUR_GEN_TEST(lt, LT, lt);
COLOUR_GEN_TEST(lt_inc, LT_INC, lt_inc);
COLOUR_GEN_TEST(gt, GT, gt);
COLOUR_GEN_TEST(gt_inc, GT_INC, gt_inc);
COLOUR_GEN_TEST(eq, EQUALS, eq);
COLOUR_GEN_TEST(inc, INCLUDES, eq);

#define COLOUR_IDENTITY_GEN_TEST(op_l, op_u, op) \
static int test_tree_ci_##op_l(avl_tree_node_t *node) \
{ \
    if (node == NULL) { \
        return 1; \
    } \
 \
    mse_card_t *card = (mse_card_t *) node->payload; \
    ASSERT(mse_colour_##op(card->colour_identity, DEFAULT_ARGUMENT)); \
 \
    ASSERT(test_tree_ci_##op_l(node->l)); \
    ASSERT(test_tree_ci_##op_l(node->r)); \
    return 1; \
} \
 \
static int test_generator_colour_identity_##op_l() \
{ \
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_COLOUR_IDENTITY; \
    size_t len = strlen(DEFAULT_ARGUMENT_STR); \
 \
    mse_set_generator_t ret; \
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_##op_u, DEFAULT_ARGUMENT_STR, len)); \
 \
    avl_tree_node_t *root = NULL; \
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool)); \
    ASSERT(tree_size(root) > 0); \
    ASSERT(test_tree_ci_##op_l(root)); \
 \
    free_tree(root); \
    mse_free_set_generator(&ret); \
    return 1; \
}

COLOUR_IDENTITY_GEN_TEST(lt, LT, lt);
COLOUR_IDENTITY_GEN_TEST(lt_inc, LT_INC, lt_inc);
COLOUR_IDENTITY_GEN_TEST(gt, GT, gt);
COLOUR_IDENTITY_GEN_TEST(gt_inc, GT_INC, gt_inc);
COLOUR_IDENTITY_GEN_TEST(eq, EQUALS, eq);
COLOUR_IDENTITY_GEN_TEST(inc, INCLUDES, eq);

SUB_TEST(test_generator_colours, {&test_generator_colours_lt, "Colours generator <="},
{&test_generator_colours_lt_inc, "Colours generator <="},
{&test_generator_colours_gt, "Colours generator >"},
{&test_generator_colours_gt_inc, "Colours generator >="},
{&test_generator_colours_eq, "Colours generator ==",},
{&test_generator_colours_inc, "Colours generator == (inc)"},
{&test_generator_colour_identity_lt, "Colour identity generator <="},
{&test_generator_colour_identity_lt_inc, "Colour identity generator <="},
{&test_generator_colour_identity_gt, "Colour identity generator >"},
{&test_generator_colour_identity_gt_inc, "Colour identity generator >="},
{&test_generator_colour_identity_eq, "Colour identity generator ==",},
{&test_generator_colour_identity_inc, "Colour identity generator == (inc)"})