#include <stdio.h>
#include <time.h>
#include "../testing_h/testing.h"
#include "./test_thread_pool.h"

static int sanity_test()
{
    return 1;
}

SUB_TEST(tests, {&sanity_test, "Sanity Test"},
{&test_thread_pool, "Test thread pool"})

int main()
{
    tzset();
    return tests() == 0 ? 0 : 1;
}
