#include "./test_formats.h"
#include "../testing_h/testing.h"
#include "mse_formats.h"

static int test_format_enum_length_is_greater_than_zero()
{
    mse_formats_t format = 0;
    ASSERT(format != MSE_FORMAT_END);

    for (; format != MSE_FORMAT_END; format++);

    ASSERT(format > 0);
    return 1;
}

static int test_format_legalities_enum_length_is_greater_than_zero()
{
    mse_format_legalities_t format_legality = 0;
    ASSERT(format_legality != MSE_FORMAT_LEGALITIES_END);

    for (; format_legality != MSE_FORMAT_LEGALITIES_END; format_legality++);

    ASSERT(format_legality > 0);
    return 1;
}

SUB_TEST(test_formats, {&test_format_enum_length_is_greater_than_zero, "Test format enum length is greater than zero"},
{&test_format_enum_length_is_greater_than_zero, "Test format legality enum length is greater than zero"})
