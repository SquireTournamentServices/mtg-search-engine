#include "./test_set.h"
#include "../testing_h/testing.h"
#include "../mse/set.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <jansson.h>

#define SET_NAME "My Cool Test Set"
#define SET_CODE "poo"
/// My birthday - please celebrate it.
#define SET_RELEASE_DATE "2002-07-10"

static int test_parse_set_json()
{
    json_t *json = json_pack("{s:s, s:s}", "name", SET_NAME, "releaseDate", SET_RELEASE_DATE);
    ASSERT(json != NULL);

    mse_set_t set;
    ASSERT(mse_parse_set_json(json, &set, SET_CODE));
    json_decref(json);

    ASSERT(memcmp(set.code, SET_CODE, strlen(SET_CODE)) == 0);
    ASSERT(strcmp(set.name, SET_NAME) == 0);
    ASSERT(set.release.tm_year == 2002 - 1900);
    ASSERT(set.release.tm_mon == 7 - 1);
    ASSERT(set.release.tm_mday == 10);

    mse_free_set(&set);
    return 1;
}

#define SET_CODE_2 "uplist"

// Some set codes are 4 characters wrong, lets test they work
static int test_parse_set_json_long_code()
{
    json_t *json = json_pack("{s:s, s:s}", "name", SET_NAME, "releaseDate", SET_RELEASE_DATE);
    ASSERT(json != NULL);

    mse_set_t set;
    ASSERT(mse_parse_set_json(json, &set, SET_CODE_2));
    json_decref(json);

    ASSERT(memcmp(set.code, SET_CODE_2, strlen(SET_CODE_2)) == 0);
    ASSERT(strcmp(set.name, SET_NAME) == 0);
    ASSERT(set.release.tm_year == 2002 - 1900);
    ASSERT(set.release.tm_mon == 7 - 1);
    ASSERT(set.release.tm_mday == 10);

    mse_free_set(&set);
    return 1;
}

static int test_write_read_set()
{
    json_t *json = json_pack("{s:s, s:s}", "name", SET_NAME, "releaseDate", SET_RELEASE_DATE);
    ASSERT(json != NULL);

    mse_set_t set;
    ASSERT(mse_parse_set_json(json, &set, SET_CODE_2));
    json_decref(json);

    ASSERT(memcmp(set.code, SET_CODE_2, strlen(SET_CODE_2)) == 0);
    ASSERT(strcmp(set.name, SET_NAME) == 0);
    ASSERT(set.release.tm_year == 2002 - 1900);
    ASSERT(set.release.tm_mon == 7 - 1);
    ASSERT(set.release.tm_mday == 10);

    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb");
    ASSERT(r != NULL);

    FILE *w = fdopen(fid[1], "wb");
    ASSERT(w != NULL);

    ASSERT(mse_write_set(w, set));
    fclose(w);

    mse_set_t set_2;
    memset(&set_2, 0xFF, sizeof(set_2));
    ASSERT(mse_read_set(r, &set_2));
    fclose(r);

    ASSERT(memcmp(set.code, set_2.code, sizeof(set.code)) == 0);
    ASSERT(strcmp(set.name, set_2.name) == 0);
    ASSERT(memcmp(&set.release, &set_2.release, sizeof(set.release)) == 0);

    mse_free_set(&set);
    mse_free_set(&set_2);
    return 1;
}

#define SET_CODE_CMP_1 "aaa"
#define SET_CODE_CMP_2 "bbb"

static int test_set_cmp()
{
    json_t *json = json_pack("{s:s, s:s}", "name", SET_NAME, "releaseDate", SET_RELEASE_DATE);
    ASSERT(json != NULL);

    mse_set_t set;
    ASSERT(mse_parse_set_json(json, &set, SET_CODE_CMP_1));

    mse_set_t set_2;
    ASSERT(mse_parse_set_json(json, &set_2, SET_CODE_CMP_2));

    ASSERT(mse_cmp_set(&set, &set_2) < 0);
    ASSERT(mse_avl_cmp_set(&set, &set_2) < 0);

    memcpy(set_2.code, set.code, sizeof(set.code));
    ASSERT(mse_cmp_set(&set, &set_2) == 0);
    ASSERT(mse_avl_cmp_set(&set, &set_2) == 0);

    set.code[0] = 'b';
    set_2.code[0] = 'a';
    ASSERT(mse_cmp_set(&set, &set_2) > 0);
    ASSERT(mse_avl_cmp_set(&set, &set_2) > 0);

    json_decref(json);
    mse_free_set(&set);
    mse_free_set(&set_2);
    return 1;
}

static int test_mse_add_card_to_set()
{
    json_t *json = json_pack("{s:s, s:s}", "name", SET_NAME, "releaseDate", SET_RELEASE_DATE);
    ASSERT(json != NULL);

    mse_set_t set;
    ASSERT(mse_parse_set_json(json, &set, SET_CODE_2));
    json_decref(json);

    ASSERT(memcmp(set.code, SET_CODE_2, strlen(SET_CODE_2)) == 0);
    ASSERT(strcmp(set.name, SET_NAME) == 0);
    ASSERT(set.release.tm_year == 2002 - 1900);
    ASSERT(set.release.tm_mon == 7 - 1);
    ASSERT(set.release.tm_mday == 10);

    mse_card_t card;
    memset(&card, 0, sizeof(card));
    ASSERT(mse_add_card_to_set(&set, &card));
    ASSERT(set.set_cards_tree != NULL);
    ASSERT(set.set_cards_tree->payload == &card);

    ASSERT(!mse_add_card_to_set(&set, &card));

    mse_free_set(&set);
    return 1;
}

SUB_TEST(test_set, {&test_parse_set_json, "Test parse set from JSON"},
{&test_parse_set_json_long_code, "Test parse set from JSON with 6 letter code"},
{&test_write_read_set, "Test write and read set"},
{&test_set_cmp, "Test set cmp function"},
{&test_mse_add_card_to_set, "Test add card to set"})
