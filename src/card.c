#include "./card.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"
#include <string.h>

int write_card(FILE *f, mtg_card_t *card)
{
    ASSERT(write_uuid(f, card->id));
    ASSERT(write_str(f, card->name));
    ASSERT(write_size_t(f, card->types_count));
    for (size_t i = 0; i < card->types_count; i++) {
        ASSERT(write_str(f, card->types[i]));
    }

    ASSERT(write_double(f, card->power));
    ASSERT(write_double(f, card->toughness));
    ASSERT(write_double(f, card->cmc));
    ASSERT(write_int(f, card->colours));
    ASSERT(write_int(f, card->colour_identity));

    ASSERT(write_size_t(f, card->set_codes_len));
    for (size_t i = 0; i < card->set_codes_len; i++) {
        ASSERT(write_set_code(f, card->set_codes[i]));
    }
    return 1;
}

int read_card(FILE *f, mtg_card_t *card)
{
    memset(card, 0, sizeof(*card));
    ASSERT(read_uuid(f, &card->id));
    ASSERT(read_str(f, &card->name));
    ASSERT(read_size_t(f, &card->types_count));
    for (size_t i = 0; i < card->types_count; i++) {
        ASSERT(read_str(f, &card->types[i]));
    }

    ASSERT(read_double(f, &card->power));
    ASSERT(read_double(f, &card->toughness));
    ASSERT(read_double(f, &card->cmc));
    ASSERT(read_int(f, &card->colours));
    ASSERT(read_int(f, &card->colour_identity));

    ASSERT(read_size_t(f, &card->set_codes_len));
    card->set_codes = malloc(sizeof(*card->set_codes) * card->set_codes_len);
    ASSERT(card->set_codes != NULL);

    for (size_t i = 0; i < card->set_codes_len; i++) {
        ASSERT(read_set_code(f, &card->set_codes[i]));
    }
    return 1;
}

void free_card(mtg_card_t *card)
{

}
