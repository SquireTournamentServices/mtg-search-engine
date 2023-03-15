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

static int sanity_test()
{
    return 1;
}

SUB_TEST(tests, {&sanity_test, "Sanity Test"},
{&test_uuid, "Test UUID"},
{&test_io_utils, "Test IO utils"},
{&test_set, "Test set"},
{&test_avl_tree, "Test AVL tree"},
{&test_card, "Test card"},
{&test_thread_pool, "Test thread pool"},
{&test_mtg_json, "Test mtg json"})


int main()
{
    tzset();
    return tests() ? 0 : 1;
}
