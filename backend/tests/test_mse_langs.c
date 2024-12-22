#include "./test_mse_langs.h"
#include "../testing_h/testing.h"
#include "mse_langs.h"

static int test_get_lang_name()
{
    for (mse_lang_codes_t code = 0; code < MSE_LANGS_END; code++) {
        ASSERT(mse_get_lang_name_str(code) != NULL);
    }
    return 1;
}

static int test_get_lang_code()
{
    for (mse_lang_codes_t code = 0; code < MSE_LANGS_END; code++) {
        ASSERT(mse_get_lang_code_str(code) != NULL);
    }
    return 1;
}

SUB_TEST(test_mse_langs, {&test_get_lang_name, "Test MSE get lang name string"},
{&test_get_lang_code, "Test MSE get lang code string"})
