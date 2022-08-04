#pragma once
#include <stdio.h>

typedef struct uuid_t {
    char bytes[16];
} uuid_t;

uuid_t from_string(char *str, int *status);
int write_uuid(FILE *f, uuid_t uuid); // Returns 0 on error
int read_uuid(FILE *f, uuid_t *uuid); // Returns 0 on error

