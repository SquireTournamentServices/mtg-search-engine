#include "./card.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdlib.h>

int parse_card_json(json_t *json, mtg_card_t *card)
{
    ASSERT(json != NULL);
    ASSERT(card != NULL);

    memset(card, 0, sizeof(*card));

    // Read name
    json_t *name_o = json_object_get(json, "name");
    ASSERT(name_o != NULL);
    ASSERT(json_is_string(name_o));
    card->name = strdup(json_string_value(name_o));

    // Read oracle
    json_t *oracle_o = json_object_get(json, "originalText");
    ASSERT(oracle_o != NULL);
    ASSERT(json_is_string(oracle_o));
    card->oracle_text = strdup(json_string_value(oracle_o));

    return 1;
}

int write_card(FILE *f, mtg_card_t *card)
{
    ASSERT(write_uuid(f, card->id));
    ASSERT(write_str(f, card->name));
    ASSERT(write_str(f, card->mana_cost));
    ASSERT(write_str(f, card->oracle_text));
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
    ASSERT(read_str(f, &card->mana_cost));
    ASSERT(read_str(f, &card->oracle_text));
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
    if (card == NULL) {
        return;
    }

    if (card->name != NULL) {
        free(card->name);
    }

    if (card->mana_cost != NULL) {
        free(card->mana_cost);
    }

    if (card->oracle_text != NULL) {
        free(card->oracle_text);
    }

    for (size_t i = 0; i < card->types_count; i++) {
        if (card->types[i] != NULL) {
            free(card->types[i]);
        }
    }

    if (card->set_codes != NULL) {
        free(card->set_codes);
    }
}
