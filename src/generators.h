#pragma once
#include <stddef.h>

typedef enum mse_set_generator_type_t {
    MSE_SET_GENERATOR_COLOUR,
    MSE_SET_GENERATOR_COLOUR_IDENTITY,
    MSE_SET_GENERATOR_NAME,
    MSE_SET_GENERATOR_ORACLE_TEXT,
    MSE_SET_GENERATOR_SET
} mse_set_generator_type_t;

typedef enum mse_set_generator_operator_t {
    /// :
    MSE_SET_GENERATOR_OP_INCLUDES,
    /// =
    MSE_SET_GENERATOR_OP_EQUALS,
    /// <
    MSE_SET_GENERATOR_OP_LT,
    /// <=
    MSE_SET_GENERATOR_OP_LT_INC,
    /// >
    MSE_SET_GENERATOR_OP_GT,
    /// >=
    MSE_SET_GENERATOR_OP_GT_INC
} mse_set_generator_operator_t;

/// The set generator and its information
typedef struct mse_set_generator_t {
    mse_set_generator_type_t generator_type;
    mse_set_generator_operator_t generator_op;
    char *argument;
} mse_set_generator_t;

/// Runs validation that the operation is defined for the given type
/// Also copies the argument string, len is the string length of the argument
int mse_init_set_generator(mse_set_generator_t *ret,
                           mse_set_generator_type_t gen_type,
                           mse_set_generator_operator_t op_type,
                           char *argument,
                           size_t len);

void mse_free_set_generator(mse_set_generator_t *gen);
