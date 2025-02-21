#include "./test_utf8_normalisation.h"
#include "../testing_h/testing.h"
#include "../mse/utf8_normalisation.h"
#include <string.h>

#define TEST_STR_NORMAL "Testing 123."

static int test_utf8_normalisation_normal_input()
{
    char *normalised = mse_normalise_utf8(TEST_STR_NORMAL);
    ASSERT(normalised != NULL);
    ASSERT(strcmp(TEST_STR_NORMAL, normalised) == 0);
    free(normalised);
    return 1;
}

static int test_utf8_normalisation_special_input_1()
{
    char *normalised = mse_normalise_utf8("Mein Code ist sheiße");
    ASSERT(normalised != NULL);
    ASSERT(strcmp("Mein Code ist sheiße", normalised) == 0);
    free(normalised);
    return 1;
}

static int test_utf8_normalisation_special_input_2()
{
    char *normalised = mse_normalise_utf8("Ça-va?");
    ASSERT(normalised != NULL);
    ASSERT(strcmp("Ca-va?", normalised) == 0);
    free(normalised);
    return 1;
}

SUB_TEST(test_utf8_normalisation, {&test_utf8_normalisation_normal_input, "Test normalisation with normal input"},
{&test_utf8_normalisation_special_input_1, "Test normalisation with special input 1"},
{&test_utf8_normalisation_special_input_2, "Test normalisation with special input 2"})
