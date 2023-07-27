#include "./re2_wrapper.h"
#include "../testing_h/testing.h"
#include <string>

int mse_re_init(mse_re_t *re, char *str)
{
    re->__re = new RE2(std::string(str));
    if (!re->__re->ok()) {
        lprintf(LOG_ERROR, "Invalid regex, cannot compile\n");
        mse_re_free(re);
        return 0;
    }
    return 1;
}

int mse_re_matches(mse_re_t *re, char *str)
{
    return RE2::FullMatch(std::string(str), *re->__re);
}

void mse_re_free(mse_re_t *re)
{
    if (re->__re != NULL) {
        delete re->__re;
    }
    memset(re, 0, sizeof(*re));
}
