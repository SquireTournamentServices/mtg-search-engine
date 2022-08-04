#include "./card.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"

/*
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
} mtg_card_t;
*/

int write_card(FILE *f, mtg_card_t card)
{
    ASSERT(write_uuid(f, card.id));
    ASSERT(write_str(f, card.name));
    ASSERT(write_size_t(f, card.types_count));
    for (size_t i = 0; i < card.types_count; i++) {
        ASSERT(write_str(f, card.types[i]));
    }

    ASSERT(write_double(f, card.power));
    ASSERT(write_double(f, card.toughness));
    ASSERT(write_double(f, card.cmc));
    ASSERT(write_int(f, card.colours));
    ASSERT(write_int(f, card.colour_identity));
    return 1;
}

int read_card(FILE *f, mtg_card_t *card)
{
    ASSERT(read_uuid(f, &card->id));
    ASSERT(read_str(f, &card->name));
    ASSERT(read_size_t(f, &card->types_count));
    for (size_t i = 0; i < &card->types_count; i++) {
        ASSERT(read_str(f, &card->types[i]));
    }

    ASSERT(read_double(f, &card->power));
    ASSERT(read_double(f, &card->toughness));
    ASSERT(read_double(f, &card->cmc));
    ASSERT(read_int(f, &card->colours));
    ASSERT(read_int(f, &card->colour_identity));
    return 1;
}

