#include "./test_card.h"
#include "../testing_h/testing.h"
#include "../src/card.h"
#include <unistd.h>
#include <stdio.h>
#include <jansson.h>

static int test_card_parse_json()
{
    FILE *f = fopen("./test_card.json", "r");
    ASSERT(f != NULL);
    json_error_t error;
    json_t *json = json_loadf(f, 0, &error);
    ASSERT(json != NULL);

    mtg_card_t card;
    ASSERT(parse_card_json(json, &card));
    free_card(&card);

    json_decref(json);
    return 1;
}

SUB_TEST(test_card, {&test_card_parse_json, "Test parse card from JSON"})
