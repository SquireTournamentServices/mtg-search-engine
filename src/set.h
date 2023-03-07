#pragma once
#include <time.h>
#include <stdio.h>
#include "./uuid.h"

typedef char mtg_set_code_t[3];

typedef struct mtg_set_t {
    uuid_t id;
    char *name;
    mtg_set_code_t code;
    struct tm release;
} mtg_set_t;

int write_set(FILE *f, mtg_set_t *set);
int read_set(FILE *f, mtg_set_t *set);

