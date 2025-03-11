#pragma once
#include <string.h>
#include <stdio.h>

typedef struct mse_uuid_t {
    unsigned char bytes[16];
} mse_uuid_t;

mse_uuid_t mse_from_string(const char *str, int *status);
/// Returns 0 on error
int mse_write_uuid(FILE *f, mse_uuid_t uuid);
/// Returns 0 on error
int mse_read_uuid(FILE *f, mse_uuid_t *uuid);

#define __MSE_FUTURE_PROOF 1
// Future prrof it a bit by subtracting one from the number
#define MIN_SET_CODE_LEN (3 - __MSE_FUTURE_PROOF)
// Future proof it a bit by adding one to the number
#define MAX_SET_CODE_LEN (6 + __MSE_FUTURE_PROOF)
typedef unsigned char mse_set_code_t[MAX_SET_CODE_LEN];

/// Reads a set code
int mse_get_set_code(const char *code, mse_set_code_t *ret);

/// Returns 0 on error
int mse_write_set_code(FILE *f, mse_set_code_t code);
/// Returns 0 on error
int mse_read_set_code(FILE *f, mse_set_code_t *code);

/// Return code is similar to that of memcmp, maybe because it uses memcmp...
#define mse_uuid_cmp(a, b) memcmp(&a, &b, sizeof(a))

/// Returns the maximum possible UUID, this can be used when doing an lookup in an AVL tree for an element where
/// the UUID makes up part of a composite key. In card.h the power and, toughness trees use a composite key of
/// the numerical value and the UUID, by using the max UUID for the thing to compare to, i.e: compare (x, max_x)
mse_uuid_t mse_max_uuid();

/// Gets the minimum UUID, similar to mse_max_uuid
mse_uuid_t mse_min_uuid();

char *mse_uuid_as_string(mse_uuid_t uuid);
