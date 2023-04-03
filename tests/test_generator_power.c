#include "./test_generators.h"
#include "../testing_h/testing.h"
#include "../src/avl_tree.h"
#include "../src/generators.h"
#include <string.h>

#define DEFAULT_ARGUMENT "5"

static int __test_generator_power(mse_set_generator_operator_t op_type)
{
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_POWER;
    size_t len = strlen(DEFAULT_ARGUMENT);

    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));

    avl_tree_node_t *root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    free_tree(root);
    mse_free_set_generator(&ret);
    return 1;
}

static int test_generator_power_eq()
{
    ASSERT(__test_generator_power(MSE_SET_GENERATOR_OP_EQUALS));
    return 1;
}

static int test_generator_power_inc()
{
    ASSERT(__test_generator_power(MSE_SET_GENERATOR_OP_INCLUDES));
    return 1;
}

SUB_TEST(test_generator_power, {&test_generator_power_eq, "Test generator power eq"},
{&test_generator_power_inc, "Test generator toughness includes"})
