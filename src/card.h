#pragma once
#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include "./uuid.h"
#include "./set.h"

// Evil macros
#ifdef british
#define color colour
#define colors colours
#endif

typedef enum mtg_colour_enum_t {
    WHITE  = 1 << 0,
    BLUE   = 1 << 1,
    BLACK  = 1 << 2,
    RED    = 1 << 3,
    GREEN  = 1 << 4
} mtg_colour_enum_t;

typedef int mtg_colour_flags_t;

#define MTG_SPECIAL_NUMERICAL_VAL NAN

typedef struct mtg_card_t {
    uuid_t id;
    char *name;
    char *mana_cost;
    size_t types_count;
    char **types;
    double power;
    double toughness;
    double cmc;
    mtg_colour_flags_t colours;
    mtg_colour_flags_t colour_identity;
    size_t set_codes_len;
    mtg_set_code_t *set_codes;
} mtg_card_t;

int write_card(FILE *f, mtg_card_t card);
int read_card(FILE *f, mtg_card_t *card);

