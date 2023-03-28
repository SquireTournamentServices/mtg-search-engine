#include <stdio.h>
#include <time.h>
#include "../testing_h/testing.h"
#include "./test_mtg_json.h"
#include "./test_thread_pool.h"
#include "./test_uuid.h"
#include "./test_io_utils.h"
#include "./test_set.h"
#include "./test_avl_tree.h"
#include "./test_card.h"
#include "./test_mse_langs.h"
#include "./test_search.h"
#include "./test_card_str_match.h"
#include "./test_generators.h"

static int sanity_test()
{
    return 1;
}

SUB_TEST(tests, {&sanity_test, "Sanity Test"},
{&test_uuid, "Test UUID"},
{&test_io_utils, "Test IO utils"},
{&test_set, "Test set"},
{&test_card, "Test card"},
{&test_thread_pool, "Test thread pool"},
{&test_mse_langs, "Test mse langs"},
{&test_search, "Test search"},
/* Data source (very important) */
{&test_mtg_json, "Test mtg json"},
/* Slow testse */
{&test_generators, "Test generators"},
{&test_card_str_match, "Test card string match"},
{&test_avl_tree, "Test AVL tree"})


int main(int argc, char** argv)
{
    tzset();
    int res = tests();
    if (argc > 1) {
        return 0;
    } else {
        return res ? 0 : 1;
    }
}
