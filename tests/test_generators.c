#include "./test_generators.h"
#include "../testing_h/testing.h"
#include "../src/generators.h"
#include <string.h>

#define DEFAULT_ARGUMENT "thopt"

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
    return 1;
}

SUB_TEST(test_generators, {&test_generator_init_free, "Test generator init free"})
