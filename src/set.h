#pragma once
#include <time.h>
#include <stdio.h>
#include <jansson.h>
#include "./uuid.h"

typedef struct mtg_set_t {
    char *name;
    mtg_set_code_t code;
    struct tm release;
} mtg_set_t;

int parse_set_json(json_t *set_node, mtg_set_t *ret, const char *code);
int write_set(FILE *f, mtg_set_t *set);
int read_set(FILE *f, mtg_set_t *set);

