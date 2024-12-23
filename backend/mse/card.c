#include "./card.h"
#include "./io_utils.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

mse_colour_enum_t mse_parse_colour(char colour)
{
    switch (colour) {
    case 'W':
    case 'w':
        return MSE_WHITE;
    case 'U':
    case 'u':
        return MSE_BLUE;
    case 'B':
    case 'b':
        return MSE_BLACK;
    case 'R':
    case 'r':
        return MSE_RED;
    case 'G':
    case 'g':
        return MSE_GREEN;
    default:
        return 0;
    }

}

mse_colour_enum_t mse_parse_colours(const char *colours)
{
    mse_colour_enum_t ret = 0;
    ASSERT(colours != NULL);
    for (size_t i = 0; colours[i]; i++) {
        ret |= mse_parse_colour(colours[i]);
    }

    return ret;
}

int mse_parse_card_json(json_t *json, mse_card_t *card)
{
    memset(card, 0, sizeof(*card));

    // Read ID
    json_t *identifiers_o = json_object_get(json, "identifiers");
    ASSERT(identifiers_o != NULL);
    ASSERT(json_is_object(identifiers_o));

    json_t *id_o = json_object_get(identifiers_o, "scryfallOracleId");
    ASSERT(id_o != NULL);
    ASSERT(json_is_string(id_o));

    int status = 0;
    card->id = mse_from_string(json_string_value(id_o), &status);
    ASSERT(status);

    // Read name
    json_t *name_o = json_object_get(json, "name");
    ASSERT(name_o != NULL);
    ASSERT(json_is_string(name_o));
    ASSERT(card->name = strdup(json_string_value(name_o)));

    // Read oracle
    json_t *oracle_o = json_object_get(json, "text");
    if (oracle_o != NULL) {
        ASSERT(json_is_string(oracle_o));
        ASSERT(card->oracle_text = strdup(json_string_value(oracle_o)));
        ASSERT(card->oracle_text_lower = mse_to_lower(card->oracle_text));
    } else {
        oracle_o = json_object_get(json, "originalText");

        if (oracle_o != NULL) {
            ASSERT(json_is_string(oracle_o));
            ASSERT(card->oracle_text = strdup(json_string_value(oracle_o)));
            ASSERT(card->oracle_text_lower = mse_to_lower(card->oracle_text));
        }
    }

    // Read mana cost
    json_t *mana_cost_o = json_object_get(json, "manaCost");
    if (mana_cost_o != NULL) {
        ASSERT(json_is_string(mana_cost_o));
        ASSERT(card->mana_cost = strdup(json_string_value(mana_cost_o)));
    }

    // Read sub types
    json_t *sub_types_o = json_object_get(json, "subtypes");
    ASSERT(sub_types_o != NULL);
    ASSERT(json_is_array(sub_types_o));

    size_t index;
    json_t *value;
    json_array_foreach(sub_types_o, index, value) {
        ASSERT(json_is_string(value));
        if (card->types == NULL) {
            ASSERT(card->types = malloc(sizeof(*card->types)));
        }

        ASSERT(card->types = realloc(card->types, sizeof(*card->types) * (1 + card->types_count)));

        char *tmp;
        ASSERT(tmp = strdup(json_string_value(value)));
        card->types[card->types_count] = tmp;
        card->types_count++;
    }

    // Read super types
    json_t *super_types_o = json_object_get(json, "supertypes");
    ASSERT(super_types_o != NULL);
    ASSERT(json_is_array(super_types_o));

    json_array_foreach(super_types_o, index, value) {
        ASSERT(json_is_string(value));
        if (card->types == NULL) {
            ASSERT(card->types = malloc(sizeof(*card->types)));
        }

        ASSERT(card->types = realloc(card->types, sizeof(*card->types) * (1 + card->types_count)));

        char *tmp;
        ASSERT(tmp = strdup(json_string_value(value)));
        card->types[card->types_count] = tmp;
        card->types_count++;
    }

    // Read "types"
    json_t *types_o = json_object_get(json, "types");
    ASSERT(types_o != NULL);
    ASSERT(json_is_array(types_o));

    json_array_foreach(types_o, index, value) {
        ASSERT(json_is_string(value));
        if (card->types == NULL) {
            ASSERT(card->types = malloc(sizeof(*card->types)));
        }

        ASSERT(card->types = realloc(card->types, sizeof(*card->types) * (1 + card->types_count)));

        char *tmp;
        ASSERT(tmp = strdup(json_string_value(value)));
        card->types[card->types_count] = tmp;
        card->types_count++;
    }

    // Read cmc
    json_t *cmc_o = json_object_get(json, "manaValue");
    ASSERT(cmc_o != NULL);
    ASSERT(json_is_number(cmc_o));
    card->cmc = json_number_value(cmc_o);

    // Read power
    json_t *p_o = json_object_get(json, "power");
    if (p_o != NULL) {
        ASSERT(json_is_string(p_o));
        card->power = atof(json_string_value(p_o));
    }

    // Read toughness
    json_t *t_o = json_object_get(json, "toughness");
    if (t_o != NULL) {
        ASSERT(json_is_string(t_o));
        card->toughness = atof(json_string_value(t_o));
    }

    // Read colours
    json_t *colours_o = json_object_get(json, "colors");
    ASSERT(colours_o != NULL);
    ASSERT(json_is_array(colours_o));

    json_array_foreach(colours_o, index, value) {
        ASSERT(json_is_string(value));
        card->colours |= mse_parse_colours(json_string_value(value));
    }

    // Read colour identity
    json_t *colour_identity_o = json_object_get(json, "colorIdentity");
    ASSERT(colour_identity_o != NULL);
    ASSERT(json_is_array(colour_identity_o));

    json_array_foreach(colour_identity_o, index, value) {
        ASSERT(json_is_string(value));
        card->colour_identity |= mse_parse_colours(json_string_value(value));
    }

    // Read sets (printings)
    json_t *sets_o = json_object_get(json, "printings");
    ASSERT(sets_o != NULL);
    ASSERT(json_is_array(sets_o));

    json_array_foreach(sets_o, index, value) {
        ASSERT(json_is_string(value));
        if (card->set_codes == NULL) {
            ASSERT(card->set_codes = malloc(sizeof(*card->set_codes)));
        }

        ASSERT(card->set_codes = realloc(card->set_codes,
                                         sizeof(*card->set_codes) * (1 + card->set_codes_count)));

        ASSERT(mse_get_set_code(json_string_value(value), &card->set_codes[card->set_codes_count]));
        card->set_codes_count++;
    }

    ASSERT(card->name_lower = mse_to_lower(card->name));

    // Read the format legalities
    ASSERT(mse_card_formats_legalities_t_from_json(json, &card->format_legalities));
    return 1;
}

int mse_write_card(FILE *f, mse_card_t card)
{
    ASSERT(mse_write_uuid(f, card.id));
    ASSERT(mse_write_str(f, card.name));
    ASSERT(mse_write_str(f, card.mana_cost));
    ASSERT(mse_write_str(f, card.oracle_text));
    ASSERT(mse_write_size_t(f, card.types_count));
    for (size_t i = 0; i < card.types_count; i++) {
        ASSERT(mse_write_str(f, card.types[i]));
    }

    ASSERT(mse_write_double(f, card.power));
    ASSERT(mse_write_double(f, card.toughness));
    ASSERT(mse_write_double(f, card.cmc));
    int tmp = card.colours;
    ASSERT(mse_write_int(f, tmp));
    tmp = card.colour_identity;
    ASSERT(mse_write_int(f, tmp));

    ASSERT(mse_write_size_t(f, card.set_codes_count));
    for (size_t i = 0; i < card.set_codes_count; i++) {
        ASSERT(mse_write_set_code(f, card.set_codes[i]));
    }
    return 1;
}

int mse_read_card(FILE *f, mse_card_t *card)
{
    memset(card, 0, sizeof(*card));
    ASSERT(mse_read_uuid(f, &card->id));
    ASSERT(mse_read_str(f, &card->name));
    ASSERT(mse_read_str(f, &card->mana_cost));
    ASSERT(mse_read_str(f, &card->oracle_text));
    ASSERT(mse_read_size_t(f, &card->types_count));

    ASSERT(card->types = malloc(sizeof(*card->types) * card->types_count));
    for (size_t i = 0; i < card->types_count; i++) {
        ASSERT(mse_read_str(f, &card->types[i]));
    }

    ASSERT(mse_read_double(f, &card->power));
    ASSERT(mse_read_double(f, &card->toughness));
    ASSERT(mse_read_double(f, &card->cmc));
    int tmp;
    ASSERT(mse_read_int(f, &tmp));
    card->colours = tmp;
    ASSERT(mse_read_int(f, &tmp));
    card->colour_identity = tmp;

    ASSERT(mse_read_size_t(f, &card->set_codes_count));
    ASSERT(card->set_codes = malloc(sizeof(*card->set_codes) * card->set_codes_count));
    for (size_t i = 0; i < card->set_codes_count; i++) {
        ASSERT(mse_read_set_code(f, &card->set_codes[i]));
    }

    ASSERT(card->name_lower = mse_to_lower(card->name));
    if (card->oracle_text != NULL) {
        ASSERT(card->oracle_text_lower = mse_to_lower(card->oracle_text));
    }
    return 1;
}

void mse_free_card(mse_card_t *card)
{
    if (card == NULL) {
        return;
    }

    if (card->name != NULL) {
        free(card->name);
    }

    if (card->name_lower != NULL) {
        free(card->name_lower);
    }

    if (card->mana_cost != NULL) {
        free(card->mana_cost);
    }

    if (card->oracle_text != NULL) {
        free(card->oracle_text);
    }

    if (card->oracle_text_lower != NULL) {
        free(card->oracle_text_lower);
    }

    if (card->types != NULL) {
        for (size_t i = 0; i < card->types_count; i++) {
            if (card->types[i] != NULL) {
                free(card->types[i]);
            }
        }
        free(card->types);
    }

    if (card->set_codes != NULL) {
        free(card->set_codes);
    }

    memset(card, 0, sizeof(*card));
}

int mse_avl_cmp_card(void * restrict a, void * restrict b)
{
    mse_card_t *ca = (mse_card_t *) a;
    mse_card_t *cb = (mse_card_t *) b;
    return mse_uuid_cmp(ca->id, cb->id);
}

int mse_avl_cmp_card_name(void * restrict a, void * restrict b)
{
    mse_card_t *ca = (mse_card_t *) a;
    mse_card_t *cb = (mse_card_t *) b;
    return strcmp(ca->name, cb->name);
}

#define MSE_CARD_DOUBLE_AVL_CMP(a, b, field) \
    mse_card_t *ca = (mse_card_t *) a; \
    mse_card_t *cb = (mse_card_t *) b; \
    if ((int) ca->field == (int) cb->field) { \
        return mse_avl_cmp_card(a, b); \
    } \
    double cmp = ca->field - cb->field; \
    if (cmp < 0) { \
        return -1; \
    } else { \
        return 1; \
    } \

int mse_avl_cmp_card_power(void *a, void *b)
{
    MSE_CARD_DOUBLE_AVL_CMP(a, b, power)
}

int mse_avl_cmp_card_toughness(void *a, void *b)
{
    MSE_CARD_DOUBLE_AVL_CMP(a, b, toughness)
}

int mse_avl_cmp_card_cmc(void *a, void *b)
{
    MSE_CARD_DOUBLE_AVL_CMP(a, b, cmc)
}
