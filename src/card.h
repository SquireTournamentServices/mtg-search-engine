#pragma once
#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include <jansson.h>
#include "./uuid.h"

// Evil macros
#ifdef british
#define color colour
#define colors colours
#endif

typedef enum mtg_colour_enum_t {
    MSE_WHITE  = 1 << 0,
    MSE_BLUE   = 1 << 1,
    MSE_BLACK  = 1 << 2,
    MSE_RED    = 1 << 3,
    MSE_GREEN  = 1 << 4
} mtg_colour_enum_t;

typedef int mtg_colour_flags_t;

/// This will parse a card's mana cost or, a colour,
/// 4{W}{U} will return MSE_BLUE | MSE_WHITE
/// W will return MSE_WHITE
mtg_colour_enum_t parse_colours(const char *colours);

#define MTG_SPECIAL_NUMERICAL_VAL NAN

typedef struct mtg_card_t {
    uuid_t id;
    char *name;
    char *mana_cost;
    char *oracle_text;
    size_t types_count;
    char **types;
    double power;
    double toughness;
    double cmc;
    mtg_colour_flags_t colours;
    mtg_colour_flags_t colour_identity;
    size_t set_codes_count;
    mtg_set_code_t *set_codes;
} mtg_card_t;

int parse_card_json(json_t *json, mtg_card_t *card);
int write_card(FILE *f, mtg_card_t card);
int read_card(FILE *f, mtg_card_t *card);
int avl_cmp_card(void *a, void *b);

void free_card(mtg_card_t *card);

// Indexes comparison functions
int avl_cmp_card_p(void *a, void *b);
int avl_cmp_card_t(void *a, void *b);
