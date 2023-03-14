#pragma once
#include <stdio.h>

typedef struct uuid_t {
    unsigned char bytes[16];
} uuid_t;

uuid_t from_string(char *str, int *status);
int write_uuid(FILE *f, uuid_t uuid); // Returns 0 on error
int read_uuid(FILE *f, uuid_t *uuid); // Returns 0 on error

typedef unsigned char mtg_set_code_t[3];

int write_set_code(FILE *f, mtg_set_code_t code); // Returns 0 on error
int read_set_code(FILE *f, mtg_set_code_t *code); // returns 0 on error

/// Return code is similar to that of memcmp, maybe because it uses memcmp...
int uuid_cmp(uuid_t *a, uuid_t *b);
