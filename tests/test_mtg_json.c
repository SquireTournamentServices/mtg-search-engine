#include "./test_mtg_json.h"
#include "../testing_h/testing.h"
#include "../src/mtg_json.h"
#include "../src/thread_pool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>

static int test_free_atomic_cards()
{
    mtg_atomic_cards_t ret;
    memset(&ret, 0, sizeof(ret));
    free_atomic_cards(&ret);
    return 1;
}

static int test_init_free()
{
    thread_pool_t pool;
    ASSERT(init_pool(&pool));

    mtg_atomic_cards_t ret;
    ASSERT(get_atomic_cards(&ret, &pool));
    free_atomic_cards(&ret);

    sleep(1); // This lets the cURL thread clean itself up before being killed
    ASSERT(free_pool(&pool));
    return 1;
}

#define TEST_TEXT "Lorem ipsum dolor sit amet, qui minim labore adipisicing minim sint cillum sint consectetur cupidatat."

static int test_curl_write_callback()
{
    int fid[2];
    ASSERT(pipe(fid) == 0);

    FILE *r = fdopen(fid[0], "rb");
    ASSERT(r != NULL);

    FILE *w = fdopen(fid[1], "wb");
    ASSERT(w != NULL);

    size_t len = sizeof(TEST_TEXT);
    ASSERT(__mtg_json_write_callback(TEST_TEXT, len, 1, (void *) w) == len);
    fclose(w);

    char buffer[1024];
    ASSERT(fgets(buffer, sizeof(buffer), r) != NULL);
    fclose(r);

    ASSERT(strcmp(buffer, TEST_TEXT) == 0);
    return 1;
}

static json_t *get_atomic_cards_from_file()
{
    FILE *f = fopen("./AllPrintings.json", "rb");
    json_error_t error;
    json_t *ret = json_loadf(f, 0, &error);

    if (ret == NULL) {
        lprintf(LOG_ERROR, "Error: %100s\n", error.text);
    }
    fclose(f);
    return ret;
}

static int test_parse_atomic_cards_sets()
{
    json_t *json = get_atomic_cards_from_file();
    ASSERT(json != NULL);

    mtg_atomic_cards_t ret;
    memset(&ret, 0, sizeof(ret));
    ASSERT(__parse_atomic_cards(&ret, json));

    // TODO: Check the sets

    free_atomic_cards(&ret);

    json_decref(json);
    return 1;
}

SUB_TEST(test_mtg_json, {&test_free_atomic_cards, "Test free zeroed atomic cards struct"},
{&test_init_free, "Test init and, free"},
{&test_curl_write_callback, "Test cURL write callback"},
{&test_parse_atomic_cards_sets, "Test that __parse_atomic_cards reads the sets correctly"})
