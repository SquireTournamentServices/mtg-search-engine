#include "./test_generators.h"
#include "../src/generators.h"
#include "../testing_h/testing.h"

static int test_generator_power_eq()
{
    return 1;
}

SUB_TEST(test_generator_power, {&test_generator_power_eq, "Test generator power eq"})
