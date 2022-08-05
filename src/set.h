#include <time.h>
#include <stdio.h>
#include "./uuid.h"

typedef struct mtg_set_t {
    uuid_t id;
    char *name;
    char code[3]; // i.e: 2xm
    struct tm release;
} mtg_set_t;

int write_set(FILE *f, mtg_set_t set);
int read_set(FILE *f, mtg_set_t *set);

