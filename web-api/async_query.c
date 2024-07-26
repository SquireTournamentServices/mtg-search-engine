#include "./async_query.h"
#include "../testing_h/testing.h"
#include <string.h>
#include <stdlib.h>
#include <jansson.h>

static int __mse_jsonify_card(json_t *json, mse_card_t *card)
{
    json_t *tmp;

    ASSERT(tmp = json_string(card->name));
    ASSERT(json_object_set(json, "name", tmp) == 0);

    if (card->mana_cost != NULL) {
        ASSERT(tmp = json_string(card->mana_cost));
        ASSERT(json_object_set(json, "mana_cost", tmp) == 0);
    }

    if (card->oracle_text != NULL) {
        ASSERT(tmp = json_string(card->oracle_text));
        ASSERT(json_object_set(json, "oracle_text", tmp) == 0);
    }

    ASSERT(tmp = json_array());
    ASSERT(json_object_set(json, "types", tmp) == 0);
    for (size_t i = 0; i < card->types_count; i++) {
        json_t *type_json = json_string(card->types[i]);
        ASSERT(type_json != NULL);
        if (!json_array_append(tmp, type_json) == 0) {
            lprintf(LOG_ERROR, "Cannot append to type array\n");
            json_decref(type_json);
            return 0;
        }
    }

    ASSERT(tmp = json_real(card->power));
    ASSERT(json_object_set(json, "power", tmp) == 0);

    ASSERT(tmp = json_real(card->toughness));
    ASSERT(json_object_set(json, "toughness", tmp) == 0);

    ASSERT(tmp = json_real(card->cmc));
    ASSERT(json_object_set(json, "cmc", tmp) == 0);

    ASSERT(tmp = json_integer(card->colours));
    ASSERT(json_object_set(json, "colours", tmp) == 0);

    ASSERT(tmp = json_integer(card->colour_identity));
    ASSERT(json_object_set(json, "colour_identity", tmp) == 0);

    ASSERT(tmp = json_array());
    ASSERT(json_object_set(json, "sets", tmp) == 0);
    for (size_t i = 0; i < card->set_codes_count; i++) {
        char buffer[sizeof(*card->set_codes) + 1];
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, card->set_codes[i], sizeof(buffer) - 1);

        json_t *type_json = json_string(buffer);
        ASSERT(type_json != NULL);
        if (!json_array_append(tmp, type_json) == 0) {
            lprintf(LOG_ERROR, "Cannot append to set code array\n");
            json_decref(type_json);
            return 0;
        }
    }
    return 1;
}

static int __mse_jsonify_search_res_impl(mse_search_result_t *res,
        json_t *json,
        mse_async_query_t *query)
{
    // Encode the response
    json_t *arr = json_array();
    ASSERT(arr);
    ASSERT(json_object_set(json, "cards", arr) == 0);

    for (size_t i = query->page_number * MSE_PAGE_SIZE;
            i < res->cards_length && i < (query->page_number + 1) * MSE_PAGE_SIZE;
            i++) {
        json_t *card = json_object();
        ASSERT(card != NULL);
        if (!__mse_jsonify_card(card, res->cards[i])) {
            lprintf(LOG_ERROR, "Cannot jsonify card\n");
            json_decref(card);
            return 0;
        }

        if (json_array_append(arr, card) != 0) {
            lprintf(LOG_ERROR, "Cannot append card\n");
            json_decref(card);
            return 0;
        }
    }

    json_t *tmp = NULL;
    ASSERT(tmp = json_integer(MSE_PAGE_SIZE));
    ASSERT(json_object_set(json, "page_size", tmp) == 0);

    ASSERT(tmp = json_integer(query->page_number));
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

    json_t *json = json_object();
    ASSERT(json != NULL);
    ASSERT(__mse_jsonify_search_res_impl(&res, json, query));
    mse_sort_search_results(&res, MSE_SORT_CARD_NAME);
    json_decref(json);
    return 1;
}

static void __mse_async_query_worker(void *data, struct mse_thread_pool_t *pool)
{
    mse_async_query_t *query = (mse_async_query_t *) data;

    int r = __mse_jsonify_search(query);
    pthread_mutex_lock(&query->lock);
    query->ready = 1;
    query->err = !r;
    pthread_mutex_unlock(&query->lock);
    mse_async_query_decref(query);
}

mse_async_query_t *mse_start_async_query(char *query, int page_number, mse_t *mse)
{
    mse_async_query_t *ret = malloc(sizeof(*ret));
    ASSERT(ret);
    memset(ret, 0, sizeof(*ret));

    pthread_mutex_t tmp = PTHREAD_MUTEX_INITIALIZER;
    ret->lock = tmp;
    ret->page_number = page_number;
    ret->query = query;
    ret->ref_count = 2;
    ret->mse = mse;

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
