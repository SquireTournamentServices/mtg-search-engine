#include "./test_async_query.h"
#include "mse/search.h"
#include <jansson.h>
#include <mse/mse.h>
#include <mse/async_query.h>
#include <pthread.h>
#include <testing_h/testing.h>
#include <unistd.h>

static mse_t state;

static int init_test()
{
    ASSERT(mse_init(&state));
    return 1;
}

static int test_async_query_base_case()
{
    mse_query_params_t params;
    params.page_number = 0;
    params.sort = MSE_SORT_CARD_NAME;
    params.sort_asc = 1;

    char *txt_query = strdup("c:r and legal:commander and type:goblin and -banned:modern and o:haste");
    ASSERT(txt_query != NULL);
    mse_async_query_t *query = mse_start_async_query(txt_query,
                               params,
                               &state);
    ASSERT(query != NULL);

    pthread_mutex_lock(&query->lock);
    ASSERT(query->ref_count == 2);
    ASSERT(query->query == txt_query);
    ASSERT(query->mse == &state);
    ASSERT(query->err == 0);
    ASSERT(query->query != NULL);
    pthread_mutex_unlock(&query->lock);

    // Waiting for query to complete
    while (!mse_async_query_poll(query));

    pthread_mutex_lock(&query->lock);
    ASSERT(query->err == 0);
    ASSERT(query->resp != NULL);
    ASSERT(strlen(query->resp) > 10);

    json_error_t err;
    json_t *json = json_loads(query->resp, strlen(query->resp), &err);
    ASSERT(json != NULL);
    json_decref(json);
    pthread_mutex_unlock(&query->lock);

    mse_async_query_decref(query);
    return 1;
}

static int test_async_query_bad_query()
{
    mse_query_params_t params;
    params.page_number = 0;
    params.sort = MSE_SORT_CARD_NAME;
    params.sort_asc = 1;

    char *txt_query = strdup("c: test");
    ASSERT(txt_query != NULL);
    mse_async_query_t *query = mse_start_async_query(txt_query,
                               params,
                               &state);
    ASSERT(query != NULL);

    pthread_mutex_lock(&query->lock);
    ASSERT(query->ref_count == 2);
    ASSERT(query->query == txt_query);
    ASSERT(query->mse == &state);
    ASSERT(query->query != NULL);
    pthread_mutex_unlock(&query->lock);

    // Waiting for query to complete
    while (!mse_async_query_poll(query));

    pthread_mutex_lock(&query->lock);
    ASSERT(query->err == 1);
    ASSERT(query->ref_count == 1);
    ASSERT(query->resp == NULL);
    pthread_mutex_unlock(&query->lock);

    mse_async_query_decref(query);
    return 1;
}

#define QUERIES 10000

static int test_lots_of_queries()
{
    mse_query_params_t params;
    params.page_number = 0;
    params.sort = MSE_SORT_CARD_NAME;
    params.sort_asc = 1;

    mse_async_query_t *queries[QUERIES];
    for (size_t i = 0; i < QUERIES; i++) {
        char *txt_query = strdup("c:r and legal:commander and type:goblin");
        ASSERT(txt_query != NULL);
        queries[i] = mse_start_async_query(txt_query,
                                           params,
                                           &state);
        ASSERT(queries[i] != NULL);
    }

    for (size_t i = 0; i < QUERIES; i++) {
        while (!mse_async_query_poll(queries[i]));
        mse_async_query_decref(queries[i]);

        pthread_mutex_lock(&queries[i]->lock);
        ASSERT(queries[i]->err == 0);
        ASSERT(queries[i]->resp != NULL);
        ASSERT(strlen(queries[i]->resp) > 10);
        pthread_mutex_unlock(&queries[i]->lock);
    }
    return 1;
}

static int free_test()
{
    mse_free(&state);
    return 1;
}

SUB_TEST(test_async_query, {&init_test, "Init test"},
{&test_async_query_base_case, "Test async query base case"},
{&test_async_query_bad_query, "Test async query bad query"},
{&test_lots_of_queries, "Test lots of queries"},
{&free_test, "Free test"})
