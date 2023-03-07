#include <stdio.h>
#include <time.h>
#include "../testing_h/testing.h"
#include "./test_mtg_json.h"
#include "./test_thread_pool.h"
#include "./test_uuid.h"

static int sanity_test()
{
    return 1;
}

SUB_TEST(tests, {&sanity_test, "Sanity Test"},
{&test_thread_pool, "Test thread pool"},
{&test_mtg_json, "Test mtg json"},
{&test_uuid, "Test uuid"})

int main()
{
    tzset();
    return tests() == 0 ? 0 : 1;
}
