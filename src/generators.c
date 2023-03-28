#include "./generators.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdlib.h>

int mse_init_set_generator(mse_set_generator_t *ret,
                           mse_set_generator_type_t gen_type,
                           mse_set_generator_operator_t op_type,
                           char *argument,
                           size_t len)
{
    memset(ret, 0, sizeof(*ret));
    ret->generator_type = gen_type;
    ret->generator_op = op_type;

    size_t buf_len = sizeof(*ret->argument) * (len + 1);
    ret->argument = malloc(buf_len);
    ASSERT(ret->argument != NULL);

    // strlcpy is not defined :(
    memset(ret->argument, 0, buf_len);
    memcpy(ret->argument, argument, len);
    return 1;
}

void mse_free_set_generator(mse_set_generator_t *gen)
{
    if (gen->argument != NULL) {
        free(gen->argument);
    }
    memset(gen, 0, sizeof(*gen));
}
