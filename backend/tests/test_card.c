#include "./test_card.h"
#include "../testing_h/testing.h"
#include "../mse/card.h"
#include "../mse/io_utils.h"
#include <unistd.h>
#include <stdio.h>
#include <jansson.h>
#include <string.h>
#include <math.h>

#define ORACLE "First strike (This creature deals combat damage before creatures without first strike.)\nWhen Ancestor's Chosen enters, you gain 1 life for each card in your graveyard."

static int __test_card_props(mse_card_t card)
{
    ASSERT(card.name != NULL);
    ASSERT(strcmp(card.name, "Ancestor's Chosen") == 0);

    ASSERT(card.name_lower != NULL);
    char *n_lower = NULL;
    ASSERT(n_lower = mse_to_lower(card.name));
    lprintf(LOG_WARNING, "'%s'\n'%s'\n", card.name_lower, n_lower);
    ASSERT(strcmp(card.name_lower, n_lower) == 0);
    free(n_lower);

    ASSERT(card.oracle_text != NULL);
    lprintf(LOG_WARNING, "'%s'\n'%s'\n", card.oracle_text, ORACLE);
    ASSERT(strcmp(card.oracle_text, ORACLE) == 0);

    ASSERT(card.oracle_text_lower != NULL);
    char *o_lower = NULL;
    ASSERT(o_lower = mse_to_lower(ORACLE));
    ASSERT(strcmp(o_lower, card.oracle_text_lower) == 0);
    free(o_lower);

    ASSERT(card.colours == card.colour_identity);
    ASSERT(card.colours == MSE_WHITE);

    ASSERT(card.mana_cost != NULL);
    ASSERT(strcmp(card.mana_cost, "{5}{W}{W}") == 0);

    ASSERT(fabs(card.cmc - 7.0) < 0.01);
    ASSERT(fabs(card.toughness - 4.0) < 0.01);
    ASSERT(fabs(card.power - 4.0) < 0.01);

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
        if (strcmp((char *) card.set_codes[i], "10e") == 0) {
            _10E = 1;
        } else if (strcmp((char *) card.set_codes[i], "jud") == 0) {
            _JUD = 1;
        } else if (strcmp((char *) card.set_codes[i], "uma") == 0) {
            _UMA = 1;
        }
    }

    ASSERT(_10E);
    ASSERT(_JUD);
    ASSERT(_UMA);

    // Card legalities
    ASSERT(card.format_legalities.commander == MSE_FORMAT_LEGALITIES_LEGAL);
    ASSERT(card.format_legalities.paupercommander == MSE_FORMAT_LEGALITIES_RESTRICTED);
    ASSERT(card.format_legalities.modern == MSE_FORMAT_LEGALITIES_LEGAL);
    return 1;
}

static int test_card_parse_json()
{
    FILE *f = fopen("./test_card.json", "r");
    ASSERT(f != NULL);
    json_error_t error;
    json_t *json = json_loadf(f, 0, &error);
    fclose(f);
    ASSERT(json != NULL);

    mse_card_t card;
    ASSERT(mse_parse_card_json(json, &card));
    ASSERT(__test_card_props(card));

    mse_free_card(&card);

    json_decref(json);
    return 1;
}

static int test_card_parse_json_no_original_text()
{
    FILE *f = fopen("./test_card_no_originalText.json", "r");
    ASSERT(f != NULL);
    json_error_t error;
    json_t *json = json_loadf(f, 0, &error);
    fclose(f);
    ASSERT(json != NULL);

    mse_card_t card;
    ASSERT(mse_parse_card_json(json, &card));
    ASSERT(__test_card_props(card));

    mse_free_card(&card);

    json_decref(json);
    return 1;
}

static int test_card_write_read()
{
    FILE *f = fopen("./test_card.json", "r");
    ASSERT(f != NULL);
    json_error_t error;
    json_t *json = json_loadf(f, 0, &error);
    fclose(f);
    ASSERT(json != NULL);

    mse_card_t card;
    ASSERT(mse_parse_card_json(json, &card));

    // Create pipe
    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb");
    ASSERT(r != NULL);

    FILE *w = fdopen(fid[1], "wb");
    ASSERT(w != NULL);

    // Test write
    ASSERT(mse_write_card(w, card));
    fclose(w);
    mse_free_card(&card);

    mse_card_t card_2;
    ASSERT(mse_read_card(r, &card_2));
    fclose(r);

    ASSERT(__test_card_props(card_2));

    mse_free_card(&card_2);

    json_decref(json);
    return 1;
}

static int test_avl_cmp_card()
{
    mse_card_t a, b;
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));

    ASSERT(mse_uuid_cmp(a.id, b.id) == 0);
    ASSERT(mse_avl_cmp_card((void *) &a, (void *) &b) == 0);

    a.id.bytes[0] = 0xFF;
    int tmp = mse_uuid_cmp(a.id, b.id);
    ASSERT(tmp > 0);
    ASSERT(mse_avl_cmp_card((void *) &a, (void *) &b) == tmp);

    a.id.bytes[0] = 0;
    b.id.bytes[0] = 0xFF;
    tmp = mse_uuid_cmp(a.id, b.id);
    ASSERT(tmp < 0);
    ASSERT(mse_avl_cmp_card((void *) &a, (void *) &b) == tmp);

    a.id = mse_min_uuid();
    b.id = mse_max_uuid();
    ASSERT(mse_avl_cmp_card(&a, &b) < 0);

    return 1;
}

static int test_card_field_cmp()
{
    mse_card_t a, b;
    memset(&a, 0, sizeof(a));
    b = a;

    ASSERT(mse_avl_cmp_card_power(&a, &b) == 0);
    ASSERT(mse_avl_cmp_card_toughness(&a, &b) == 0);
    ASSERT(mse_avl_cmp_card_cmc(&a, &b) == 0);

    a.id = mse_min_uuid();
    b.id = mse_max_uuid();

    ASSERT(mse_avl_cmp_card_power(&a, &b) < 0);
    ASSERT(mse_avl_cmp_card_toughness(&a, &b) < 0);
    ASSERT(mse_avl_cmp_card_cmc(&a, &b) < 0);

    a.power = b.power + 3;
    ASSERT(mse_avl_cmp_card_power(&a, &b) > 0);

    a.power = b.power - 3;
    ASSERT(mse_avl_cmp_card_power(&a, &b) < 0);
    return 1;
}

static int test_colour_cmp_lt()
{
    ASSERT(mse_colour_lt(MSE_WHITE, MSE_WHITE | MSE_RED));
    ASSERT(!mse_colour_lt(MSE_WHITE, MSE_WHITE));
    ASSERT(!mse_colour_lt(MSE_WHITE | MSE_BLUE, MSE_WHITE | MSE_RED));
    ASSERT(mse_colour_lt(MSE_WHITE | MSE_BLUE, MSE_WUBRG));
    return 1;
}

static int test_colour_cmp_lt_inc()
{
    ASSERT(mse_colour_lt_inc(MSE_WHITE, MSE_WHITE | MSE_RED));
    ASSERT(mse_colour_lt_inc(MSE_WHITE, MSE_WHITE));
    ASSERT(mse_colour_lt_inc(MSE_WHITE | MSE_BLUE, MSE_WHITE | MSE_RED));
    ASSERT(mse_colour_lt_inc(MSE_WHITE | MSE_BLUE, MSE_WUBRG));
    ASSERT(mse_colour_lt_inc(MSE_WUBRG, MSE_WUBRG));
    ASSERT(mse_colour_lt_inc(0, MSE_WHITE));
    ASSERT(!mse_colour_lt_inc(MSE_WHITE, 0));
    ASSERT(!mse_colour_lt_inc(MSE_WUBRG, MSE_WHITE));
    return 1;
}

static int test_colour_cmp_gt()
{
    ASSERT(mse_colour_gt(MSE_WHITE | MSE_RED, MSE_WHITE));
    ASSERT(!mse_colour_gt(MSE_WHITE, MSE_WHITE));
    ASSERT(!mse_colour_gt(MSE_WHITE | MSE_BLUE, MSE_WHITE | MSE_RED));
    ASSERT(mse_colour_gt(MSE_WUBRG, MSE_WHITE | MSE_BLUE));
    return 1;
}

static int test_colour_cmp_gt_inc()
{
    ASSERT(mse_colour_gt_inc(MSE_WHITE | MSE_RED, MSE_WHITE));
    ASSERT(mse_colour_gt_inc(MSE_WHITE, MSE_WHITE));
    ASSERT(mse_colour_gt_inc(MSE_WHITE | MSE_BLUE, MSE_WHITE | MSE_RED));
    ASSERT(!mse_colour_gt_inc(MSE_BLUE, MSE_WHITE | MSE_BLUE));
    ASSERT(mse_colour_gt_inc(MSE_WUBRG, MSE_WHITE | MSE_BLUE));
    ASSERT(mse_colour_gt_inc(MSE_WUBRG, MSE_WUBRG));
    return 1;
}

static int test_colour_cmp_eq()
{
    ASSERT(mse_colour_eq(MSE_WHITE, MSE_WHITE));
    ASSERT(!mse_colour_eq(MSE_RED, MSE_WHITE));
    ASSERT(mse_colour_eq(MSE_WUBRG, MSE_WUBRG));
    return 1;
}

SUB_TEST(test_card, {&test_card_parse_json, "Test parse card from JSON"},
{&test_card_parse_json_no_original_text, "Test parse card from JSON (no original text)"},
{&test_card_write_read, "Test card read and, write"},
{&test_avl_cmp_card, "Test card avl cmp function"},
{&test_card_field_cmp, "Test card field cmp"},
{&test_colour_cmp_lt, "Test colour <"},
{&test_colour_cmp_lt_inc, "Test colour <="},
{&test_colour_cmp_gt, "Test colour >"},
{&test_colour_cmp_gt_inc, "Test colour >="},
{&test_colour_cmp_eq, "Test colour =="})
