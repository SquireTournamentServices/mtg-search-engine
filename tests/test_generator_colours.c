#include "./test_generators.h"
#include "../src/card.h"
#include "../src/generators.h"
#include "../src/avl_tree.h"
#include "../testing_h/testing.h"
#include <string.h>

#define DEFAULT_ARGUMENT (MSE_WHITE | MSE_BLUE)
#define DEFAULT_ARGUMENT_STR "WU"

#define GEN_TEST(op_l, op_u, op) \
static int test_tree_##op_l(avl_tree_node_t *node) \
{ \
    if (node == NULL) { \
        return 1; \
    } \
 \
    mse_card_t *card = (mse_card_t *) node->payload; \
    ASSERT(card->colours op DEFAULT_ARGUMENT); \
 \
    ASSERT(test_tree_##op_l(node->l)); \
    ASSERT(test_tree_##op_l(node->r)); \
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
    ASSERT(test_tree_##op_l(root)); \
 \
    free_tree(root); \
    mse_free_set_generator(&ret); \
    return 1; \
}

GEN_TEST(lt, LT, <);
GEN_TEST(lt_inc, LT_INC, <=);
GEN_TEST(gt, GT, >);
GEN_TEST(gt_inc, GT_INC, >=);
GEN_TEST(eq, EQUALS, ==);
GEN_TEST(inc, INCLUDES, ==);

SUB_TEST(test_generator_colours, {&test_generator_colours_lt, "Colours generator <="},
{&test_generator_colours_lt_inc, "Colours generator <="},
{&test_generator_colours_gt, "Colours generator >"},
{&test_generator_colours_gt_inc, "Colours generator >="},
{&test_generator_colours_eq, "Colours generator ==",},
{&test_generator_colours_inc, "Colours generator == (inc)"})
