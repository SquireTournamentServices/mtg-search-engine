#include "./test_mtg_json.h"
#include "../testing_h/testing.h"
#include "../src/mtg_json.h"
#include "../src/thread_pool.h"

static int test_init_free()
{
    thread_pool_t pool;
    ASSERT(init_pool(&pool));

    mtg_atomic_cards_t ret;
    ASSERT(get_atomic_cards(&ret, &pool));
    free_atomic_cards(&ret);

    ASSERT(free_pool(&pool));
    return 1;

    return 1;
}

SUB_TEST(test_mtg_json, {&test_init_free, "Test init and, free"})
