#pragma once
#ifdef __cplusplus
#include <re2/re2.h>
#endif

typedef struct mse_re_t {
#ifdef __cplusplus
    RE2 *__re;
#else
    void *__re;
#endif
} mse_re_t;

#ifdef __cplusplus
extern "C" {
#endif

int mse_re_init(mse_re_t *re, char *str);
int mse_re_matches(mse_re_t *re, char *str);
void mse_re_free(mse_re_t *re);

#ifdef __cplusplus
};
#endif
