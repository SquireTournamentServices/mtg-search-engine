#include "./test_generators.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include "../src/generators.h"
#include <string.h>
#include <math.h>

#define BAD_ARGUMENT "12341234123412341234sjaskldjaslkdjasld :()"
#define DEFAULT_ARGUMENT "5"
#define DEFAULT_ARGUMENT_DOUBLE atof(DEFAULT_ARGUMENT)

static int found_eq;

static int __test_generator_power(mse_set_generator_operator_t op_type, int (*test_tree)(avl_tree_node_t *node))
{
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_POWER;
    size_t len = strlen(DEFAULT_ARGUMENT);

    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));

    avl_tree_node_t *root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree(root));
    free_tree(root);
    mse_free_set_generator(&ret);
    return 1;
}

static int test_generator_power_invalid_arg()
{
    mse_set_generator_operator_t op_type = MSE_SET_GENERATOR_OP_LT;
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_POWER;
    size_t len = strlen(BAD_ARGUMENT);

    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, BAD_ARGUMENT, len));

    avl_tree_node_t *root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool) == 0);
    mse_free_set_generator(&ret);
    return 1;
}

static int test_tree_power_eq(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    double power = ((mtg_card_t *) node->payload)->power;
    lprintf(LOG_INFO, "Expceted: \t%lf, Actual: \t%lf\n", DEFAULT_ARGUMENT_DOUBLE, power);
    ASSERT((int) DEFAULT_ARGUMENT_DOUBLE == (int) power);
    ASSERT(test_tree_power_eq(node->l));
    ASSERT(test_tree_power_eq(node->r));
    return 1;
}

static int test_generator_power_eq()
{
    ASSERT(__test_generator_power(MSE_SET_GENERATOR_OP_EQUALS, test_tree_power_eq));
    return 1;
}

static int test_generator_power_inc()
{
    ASSERT(__test_generator_power(MSE_SET_GENERATOR_OP_INCLUDES, test_tree_power_eq));
    return 1;
}

static int test_tree_power_lt(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    double power = ((mtg_card_t *) node->payload)->power;
    ASSERT(DEFAULT_ARGUMENT_DOUBLE < power);
    ASSERT(fabs(DEFAULT_ARGUMENT_DOUBLE - power) > 0.01);
    ASSERT(test_tree_power_lt(node->l));
    ASSERT(test_tree_power_lt(node->r));
    return 1;
}

static int test_generator_power_lt()
{
    ASSERT(__test_generator_power(MSE_SET_GENERATOR_OP_LT, test_tree_power_lt));
    return 1;
}

static int test_tree_power_lt_inc(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    double power = ((mtg_card_t *) node->payload)->power;
    ASSERT(DEFAULT_ARGUMENT_DOUBLE <= power);
    found_eq |= (int) DEFAULT_ARGUMENT_DOUBLE == (int) power;
    ASSERT(test_tree_power_lt_inc(node->l));
    ASSERT(test_tree_power_lt_inc(node->r));
    return 1;
}

static int test_generator_power_lt_inc()
{
    found_eq = 0;
    ASSERT(__test_generator_power(MSE_SET_GENERATOR_OP_LT_INC, &test_tree_power_lt_inc));
    ASSERT(found_eq);
    return 1;
}

static int test_tree_power_gt(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    double power = ((mtg_card_t *) node->payload)->power;
    ASSERT(DEFAULT_ARGUMENT_DOUBLE > power);
    ASSERT(fabs(DEFAULT_ARGUMENT_DOUBLE - power) > 0.01);
    ASSERT(test_tree_power_gt(node->l));
    ASSERT(test_tree_power_gt(node->r));
    return 1;
}

static int test_generator_power_gt()
{
    ASSERT(__test_generator_power(MSE_SET_GENERATOR_OP_GT, &test_tree_power_gt));
    return 1;
}

static int test_tree_power_gt_inc(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    double power = ((mtg_card_t *) node->payload)->power;
    ASSERT(DEFAULT_ARGUMENT_DOUBLE >= power);
    found_eq |= (int) DEFAULT_ARGUMENT_DOUBLE == (int) power;
    ASSERT(test_tree_power_gt_inc(node->l));
    ASSERT(test_tree_power_gt_inc(node->r));
    return 1;
}

static int test_generator_power_gt_inc()
{
    found_eq = 0;
    ASSERT(__test_generator_power(MSE_SET_GENERATOR_OP_GT_INC, &test_tree_power_gt_inc));
    ASSERT(found_eq);
    return 1;
}

SUB_TEST(test_generator_power, {&test_generator_power_invalid_arg, "Test generator power invalid arg"},
{&test_generator_power_eq, "Test generator power ="},
{&test_generator_power_inc, "Test generator power :"},
{&test_generator_power_lt, "Test genreator power <"},
{&test_generator_power_lt_inc, "Test genreator power <="},
{&test_generator_power_gt, "Test genreator power >"},
{&test_generator_power_gt_inc, "Test genreator power >="})
