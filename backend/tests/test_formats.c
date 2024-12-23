#include "./test_formats.h"
#include "../testing_h/testing.h"
#include "mse_formats.h"
#include <string.h>

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

static int test_format_enum_to_str()
{
    for (mse_formats_t format = 0; format < MSE_FORMAT_END; format++) {
        const char *format_str = mse_formats_t_as_str(format);

        mse_formats_t format_lookup;
        ASSERT(mse_str_as_mse_formats_t(format_str, &format_lookup));
        ASSERT(format == format_lookup);

        ASSERT(strcmp(format_str, mse_formats_t_as_str(format_lookup)) == 0);
    }

    return 1;
}

static int test_format_legalities_enum_to_str()
{
    for (mse_format_legalities_t legality = 0; legality < MSE_FORMAT_LEGALITIES_END; legality++) {
        const char *legality_str = mse_format_legalities_t_as_str(legality);

        mse_format_legalities_t legality_lookup;
        ASSERT(mse_str_as_mse_format_legalities_t(legality_str, &legality_lookup));
        ASSERT(legality == legality_lookup);

        ASSERT(strcmp(legality_str, mse_format_legalities_t_as_str(legality_lookup)) == 0);
    }

    return 1;
}

SUB_TEST(test_formats, {&test_format_enum_length_is_greater_than_zero, "Test format enum length is greater than zero"},
{&test_format_enum_length_is_greater_than_zero, "Test format legality enum length is greater than zero"},
{&test_format_enum_to_str, "Test formats to string (and reverse)"},
{&test_format_legalities_enum_to_str, "Test format legalities to string (and reverse)"})
