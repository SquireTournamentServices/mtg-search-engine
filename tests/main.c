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
#include "./test_card_txt_field_trie.h"
#include "./test_generators.h"
#include "./test_interpretor.h"
#include "./test_parser.h"
#include "./test_save.h"
#include "./system_test.h"

static int sanity_test()
{
    return 1;
}

SUB_TEST(tests, {&sanity_test, "Sanity Test"},
         /* Test common funcs and types */
{&test_uuid, "Test UUID"},
{&test_io_utils, "Test IO utils"},
{&test_avl_tree, "Test AVL tree"},
{&test_thread_pool, "Test thread pool"},
/* Test domain specific types and funcs */
{&test_set, "Test set"},
{&test_card, "Test card"},
{&test_card_txt_field_trie, "Test card field trie"},
{&test_mse_langs, "Test mse langs"},
{&test_search, "Test search"},
/* Data source (very important) */
{&test_mse_json, "Test mtg json"},
{&test_save, "Test saving (and reading) MTG json"},
/* Tests that require the data source */
{&test_card_str_match, "Test card string match"},
{&test_generators, "Test generators"},
{&test_interpretor, "Test interpretor"},
{&test_parser, "Test parser"},
{&test_system, "System Test"})

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
