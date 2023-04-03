#include "./test_generators.h"
#include "../testing_h/testing.h"
#include "../src/generators.h"
#include <string.h>

#define DEFAULT_ARGUMENT "12345"

static int __test_all_ops(mse_set_generator_type_t gen_type)
{
    mse_set_generator_operator_t op_type = MSE_SET_GENERATOR_OP_LT_INC;
    size_t len = strlen(DEFAULT_ARGUMENT);

    // <=
    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // <
    op_type = MSE_SET_GENERATOR_OP_LT;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // >
    op_type = MSE_SET_GENERATOR_OP_GT;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // >=
    op_type = MSE_SET_GENERATOR_OP_GT_INC;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // :
    op_type = MSE_SET_GENERATOR_OP_INCLUDES;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // =
    op_type = MSE_SET_GENERATOR_OP_EQUALS;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);
    return 1;
}

static int test_colours_validation()
{
    ASSERT(__test_all_ops(MSE_SET_GENERATOR_COLOUR));
    return 1;
}

static int test_colour_indentity_validation()
{
    ASSERT(__test_all_ops(MSE_SET_GENERATOR_COLOUR_IDENTITY));
    return 1;
}

static int test_power_validation()
{
    ASSERT(__test_all_ops(MSE_SET_GENERATOR_POWER));
    return 1;
}

static int test_toughness_validation()
{
    ASSERT(__test_all_ops(MSE_SET_GENERATOR_TOUGHNESS));
    return 1;
}

static int __test_includes_ops(mse_set_generator_type_t gen_type)
{
    mse_set_generator_operator_t op_type = MSE_SET_GENERATOR_OP_LT_INC;
    size_t len = strlen(DEFAULT_ARGUMENT);

    // <=
    mse_set_generator_t ret;
    ASSERT(!mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // <
    op_type = MSE_SET_GENERATOR_OP_LT;
    ASSERT(!mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // >
    op_type = MSE_SET_GENERATOR_OP_GT;
    ASSERT(!mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // >=
    op_type = MSE_SET_GENERATOR_OP_GT_INC;
    ASSERT(!mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // :
    op_type = MSE_SET_GENERATOR_OP_INCLUDES;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);

    // =
    op_type = MSE_SET_GENERATOR_OP_EQUALS;
    ASSERT(mse_init_set_generator(&ret, gen_type, op_type, DEFAULT_ARGUMENT, len));
    mse_free_set_generator(&ret);
    return 1;
}

static int test_name_validation()
{
    ASSERT(__test_includes_ops(MSE_SET_GENERATOR_NAME));
    return 1;
}

static int test_oracle_validation()
{
    ASSERT(__test_includes_ops(MSE_SET_GENERATOR_ORACLE_TEXT));
    return 1;
}

static int test_set_validation()
{
    ASSERT(__test_includes_ops(MSE_SET_GENERATOR_SET));
    return 1;
}

SUB_TEST(test_generator_validation, {&test_colours_validation, "Test colour validation"},
{&test_colour_indentity_validation, "Test colour identity validation"},
{&test_name_validation, "Test name validation"},
{&test_oracle_validation, "Test oracle validation"},
{&test_set_validation, "Test set validation"},
{&test_power_validation, "Test power validation"},
{&test_toughness_validation, "Test toughness validation"})
