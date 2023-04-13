#include "./generators.h"
#include "./generator_double_fields.h"
#include "./generator_set.h"
#include "./generator_oracle.h"
#include "./generator_name.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdlib.h>

int mse_init_set_generator(mse_set_generator_t *ret,
                           mse_set_generator_type_t gen_type,
                           mse_set_generator_operator_t op_type,
                           char *argument,
                           size_t len)
{
    ASSERT(__mse_validate_generator_op_combo(gen_type, op_type));
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

static int __mse_is_op_set_includes(mse_set_generator_operator_t op_type)
{
    if (op_type == MSE_SET_GENERATOR_OP_EQUALS) {
        return 1;
    }

    if (op_type == MSE_SET_GENERATOR_OP_INCLUDES) {
        return 1;
    }
    return 0;
}

int __mse_validate_generator_op_combo(mse_set_generator_type_t gen_type,
                                      mse_set_generator_operator_t op_type)
{
    switch(gen_type) {
    case MSE_SET_GENERATOR_COLOUR:
    case MSE_SET_GENERATOR_CMC:
    case MSE_SET_GENERATOR_COLOUR_IDENTITY:
    case MSE_SET_GENERATOR_POWER:
    case MSE_SET_GENERATOR_TOUGHNESS:
        return 1;
    case MSE_SET_GENERATOR_NAME:
        return __mse_is_op_set_includes(op_type);
    case MSE_SET_GENERATOR_ORACLE_TEXT:
        return __mse_is_op_set_includes(op_type);
    case MSE_SET_GENERATOR_SET:
        return __mse_is_op_set_includes(op_type);
    }
    return 0;
}


int mse_generate_set(mse_set_generator_t *gen,
                     avl_tree_node_t **res,
                     mtg_all_printings_cards_t *cards,
                     thread_pool_t *pool)
{
    switch(gen->generator_type) {
    case MSE_SET_GENERATOR_COLOUR:
        return 0;
    case MSE_SET_GENERATOR_COLOUR_IDENTITY:
        return 0;
    case MSE_SET_GENERATOR_NAME:
        return mse_generate_set_name(gen, res, cards, pool);
    case MSE_SET_GENERATOR_ORACLE_TEXT:
        return mse_generate_set_oracle(gen, res, cards, pool);
    case MSE_SET_GENERATOR_SET:
        return mse_generate_set_set(gen, res, cards);
    case MSE_SET_GENERATOR_POWER:
        return mse_generate_set_power(gen, res, cards);
    case MSE_SET_GENERATOR_TOUGHNESS:
        return mse_generate_set_toughness(gen, res, cards);
    case MSE_SET_GENERATOR_CMC:
        return mse_generate_set_cmc(gen, res, cards);
    }
    return 1;
}
