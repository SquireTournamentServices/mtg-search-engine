#include <unistd.h>
#include <math.h>
#include "./test_thread_pool.h"
#include "../testing_h/testing.h"
#include "../src/thread_pool.h"

static int test_pool_init_free()
{
    thread_pool_t pool;
    ASSERT(init_pool(&pool));
    ASSERT(pool.threads != NULL);
    ASSERT(pool.threads_count > 1);
    ASSERT(free_pool(&pool));
    return 1;
}

static void basic_calc(void *data, thread_pool_t *pool)
{
    size_t num = (size_t) data;
    if (num < 1000) {
        num = 1000;
    }
    for (int i = 0; i < 1000000; i++) {
        pow(num, num);
    }

    if (pool == NULL) {
        lprintf(LOG_ERROR, "NULL pool in worker thead\n");
        exit(1);
    }
}

static int test_pool_enqueue()
{
    thread_pool_t pool;
    ASSERT(init_pool(&pool));

    for (size_t i = 0; i < 100 * pool.threads_count; i++) {
        task_t task = {(void *) 5L, &basic_calc};
        ASSERT(task_queue_enqueue(&pool.queue, task));
    }

    ASSERT(free_pool(&pool));
    return 1;
}

SUB_TEST(test_thread_pool, {&test_pool_init_free, "Thread pool init free"},
{&test_pool_enqueue, "Thread pool enqueue"})

