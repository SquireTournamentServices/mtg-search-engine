#pragma once
#include <limits.h>

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
    char *name;
    char *oracle;
    double power;
    double toughness;
    double cmc;
    mtg_colour_flags_t colours;
    mtg_colour_flags_t colour_identity;
} mtg_card_t;

