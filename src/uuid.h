#pragma once
#include <stdio.h>

typedef struct uuid_t {
    unsigned char bytes[16];
} uuid_t;

uuid_t from_string(const char *str, int *status);
int write_uuid(FILE *f, uuid_t uuid); // Returns 0 on error
int read_uuid(FILE *f, uuid_t *uuid); // Returns 0 on error

#define __MSE_FUTURE_PROOF 1
// Future prrof it a bit by subtracting one from the number
#define MIN_SET_CODE_LEN (3 - __MSE_FUTURE_PROOF)
// Future proof it a bit by adding one to the number
#define MAX_SET_CODE_LEN (6 + __MSE_FUTURE_PROOF)
typedef unsigned char mtg_set_code_t[MAX_SET_CODE_LEN];

/// Reads a set code
int get_set_code(const char *code, mtg_set_code_t *ret);

int write_set_code(FILE *f, mtg_set_code_t code); // Returns 0 on error
int read_set_code(FILE *f, mtg_set_code_t *code); // returns 0 on error

/// Return code is similar to that of memcmp, maybe because it uses memcmp...
int uuid_cmp(uuid_t a, uuid_t b);
