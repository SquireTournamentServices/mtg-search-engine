#include "./test_set.h"
#include "../testing_h/testing.h"
#include "../src/set.h"
#include <string.h>
#include <jansson.h>

#define SET_NAME "My Cool Test Set"
#define SET_CODE "POO"
/// My birthday - please celebrate it.
#define SET_RELEASE_DATE "2002-07-10"

static int test_parse_set_json()
{
    json_t *json = json_pack("{s:s, s:s}", "name", SET_NAME, "releaseDate", SET_RELEASE_DATE);
    ASSERT(json != NULL);

    mtg_set_t set;
    ASSERT(parse_set_json(json, &set, SET_CODE));
    json_decref(json);

    ASSERT(memcmp(set.code, SET_CODE, strlen(SET_CODE)) == 0);
    ASSERT(strcmp(set.name, SET_NAME) == 0);
    ASSERT(set.release.tm_year == 2002 - 1900);
    ASSERT(set.release.tm_mon == 7 - 1);
    ASSERT(set.release.tm_mday == 10);

    free_set(&set);
    return 1;
}

#define SET_CODE_2 "POOP"

// Some set codes are 4 characters wrong, lets test they work
static int test_parse_set_json_long_code()
{
    json_t *json = json_pack("{s:s, s:s}", "name", SET_NAME, "releaseDate", SET_RELEASE_DATE);
    ASSERT(json != NULL);

    mtg_set_t set;
    ASSERT(parse_set_json(json, &set, SET_CODE_2));
    json_decref(json);

    ASSERT(memcmp(set.code, SET_CODE_2, strlen(SET_CODE_2)) == 0);
    ASSERT(strcmp(set.name, SET_NAME) == 0);
    ASSERT(set.release.tm_year == 2002 - 1900);
    ASSERT(set.release.tm_mon == 7 - 1);
    ASSERT(set.release.tm_mday == 10);

    free_set(&set);
    return 1;
}

SUB_TEST(test_set, {&test_parse_set_json, "Test parse set from JSON"},
{&test_parse_set_json_long_code, "Test parse set from JSON with 4 letter code"})
