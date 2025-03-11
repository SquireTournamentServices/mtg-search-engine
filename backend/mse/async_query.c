#include "./async_query.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdlib.h>
#include <jansson.h>

#define __mse_jsonify_search_res_card() \
   if (json_array_append(arr, res->cards[index]->json) != 0) { \
       lprintf(LOG_ERROR, "Cannot append card \n"); \
       return 0; \
   }

static int __mse_jsonify_search_res_impl(mse_search_result_t *res,
        json_t *json,
        mse_async_query_t *query)
{
    // Encode the response
    json_t *arr = json_array();
    ASSERT(arr);
    ASSERT(json_object_set(json, "cards", arr) == 0);


    for (size_t i = query->params.page_number * MSE_PAGE_SIZE;
            i < res->cards_length && i <= (query->params.page_number + 1) * MSE_PAGE_SIZE;
            i++) {

        size_t index = i;
        if (!query->params.sort_asc) {
            index = res->cards_length - index - 1;
        }
        __mse_jsonify_search_res_card()
    }

    json_t *tmp = NULL;
    ASSERT(tmp = json_integer(MSE_PAGE_SIZE));
    ASSERT(json_object_set(json, "page_size", tmp) == 0);

    ASSERT(tmp = json_integer(query->params.page_number));
    ASSERT(json_object_set(json, "page", tmp) == 0);

    ASSERT(tmp = json_integer(res->cards_length));
    ASSERT(json_object_set(json, "cards_total", tmp) == 0);

    ASSERT(query->resp = json_dumps(json, 0));
    return 1;
}

static int __mse_jsonify_search(mse_async_query_t *query)
{
    mse_search_result_t res;
    ASSERT(mse_search(query->mse, &res, query->query));
    mse_sort_search_results(&res, query->params.sort);

    json_t *json = json_object();
    ASSERT(json != NULL);
    ASSERT(__mse_jsonify_search_res_impl(&res, json, query));
    json_decref(json);

    mse_free_search_results(&res);
    return 1;
}

static void __mse_async_query_worker(void *data, struct mse_thread_pool_t *pool)
{
    mse_async_query_t *query = (mse_async_query_t *) data;

    int r = __mse_jsonify_search(query);
    clock_gettime(CLOCK_REALTIME, &query->stop);

    pthread_mutex_lock(&query->lock);
    query->ready = 1;
    query->err = !r;
    pthread_mutex_unlock(&query->lock);
    mse_async_query_decref(query);
}

mse_async_query_t *mse_start_async_query(char *query, mse_query_params_t params, mse_t *mse)
{
    mse_async_query_t *ret = malloc(sizeof(*ret));
    ASSERT(ret);
    memset(ret, 0, sizeof(*ret));

    pthread_mutex_t tmp = PTHREAD_MUTEX_INITIALIZER;
    ret->lock = tmp;
    ret->params = params;
    ret->query = query;
    ret->ref_count = 2;
    ret->mse = mse;
    clock_gettime(CLOCK_REALTIME, &ret->start);

    mse_task_t task;
    task.data = ret;
    task.exec_func = &__mse_async_query_worker;

    if (!mse_task_queue_enqueue(&mse->pool.queue, task)) {
        lprintf(LOG_ERROR, "Cannot enqueue task\n");
        mse_async_query_decref(ret);
        return NULL;
    }

    return ret;
}

int mse_async_query_poll(mse_async_query_t *query)
{
    pthread_mutex_lock(&query->lock);
    int ready = query->ready;
    pthread_mutex_unlock(&query->lock);
    return ready;
}

void mse_async_query_decref(mse_async_query_t *query)
{
    pthread_mutex_lock(&query->lock);
    int ref_count = --query->ref_count;
    pthread_mutex_unlock(&query->lock);

    if (ref_count > 0) {
        return;
    }

    if (query->query != NULL) {
        free(query->query);
    }

    if (query->resp != NULL) {
        free(query->resp);
    }

    pthread_mutex_destroy(&query->lock);
    free(query);
}
