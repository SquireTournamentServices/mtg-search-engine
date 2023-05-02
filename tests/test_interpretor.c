#include "./test_interpretor.h"
#include "../testing_h/testing.h"
#include "../src/interpretor.h"
#include <string.h>

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

SUB_TEST(test_interpretor, {&test_init_free_operator, "Test init free for operator node"},
{&test_init_free_generator, "Test init free for generator node"},
{&test_recursive_free, "Test recursive free"})
