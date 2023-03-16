#include "./test_card.h"
#include "../testing_h/testing.h"
#include "../src/card.h"
#include <unistd.h>
#include <stdio.h>
#include <jansson.h>
#include <string.h>
#include <math.h>

#define ORACLE "First strike (This creature deals combat damage before creatures without first strike.)\nWhen Ancestor's Chosen comes into play, you gain 1 life for each card in your graveyard."

static int test_card_parse_json()
{
    FILE *f = fopen("./test_card.json", "r");
    ASSERT(f != NULL);
    json_error_t error;
    json_t *json = json_loadf(f, 0, &error);
    ASSERT(json != NULL);

    mtg_card_t card;
    ASSERT(parse_card_json(json, &card));

    ASSERT(card.name != NULL);
    ASSERT(strcmp(card.name, "Ancestor's Chosen") == 0);

    ASSERT(card.oracle_text != NULL);
    ASSERT(strcmp(card.oracle_text, ORACLE) == 0);

    ASSERT(card.colours == card.colour_identity);
    ASSERT(card.colours == MSE_WHITE);

    ASSERT(card.mana_cost != NULL);
    ASSERT(strcmp(card.mana_cost, "{5}{W}{W}") == 0);

    ASSERT(fabs(card.cmc - 7.0) < 0.01);

    // Types
    int human = 0;
    int creature = 0;
    int cleric = 0;

    ASSERT(card.types_count == 3);
    ASSERT(card.types != NULL);
    for (size_t i = 0; i < card.types_count; i++) {
        if (strcmp(card.types[i], "Human") == 0) {
            human = 1;
        } else if (strcmp(card.types[i], "Cleric") == 0) {
            cleric = 1;
        } else if (strcmp(card.types[i], "Creature") == 0) {
            creature = 1;
        }
    }
    ASSERT(human);
    ASSERT(creature);
    ASSERT(cleric);

    // Sets
    int _10E = 0;
    int _JUD = 0;
    int _UMA = 0;

    ASSERT(card.set_codes_count == 3);
    ASSERT(card.set_codes != NULL);
    for (size_t i = 0; i < card.set_codes_count; i++) {
        if (strcmp((char *) card.set_codes[i], "10E") == 0) {
            _10E = 1;
        } else if (strcmp((char *) card.set_codes[i], "JUD") == 0) {
            _JUD = 1;
        } else if (strcmp((char *) card.set_codes[i], "UMA") == 0) {
            _UMA = 1;
        }
    }

    ASSERT(_10E);
    ASSERT(_JUD);
    ASSERT(_UMA);

    free_card(&card);

    json_decref(json);
    return 1;
}

SUB_TEST(test_card, {&test_card_parse_json, "Test parse card from JSON"})
