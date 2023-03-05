#pragma once
#include "./thread_pool.h"
#include <jansson.h>

#define MSE_GZ_SUFFIX ".gz"
#define ATOMIC_CARDS_URL "https://mtgjson.com/api/v5/AtomicCards.json"

typedef struct mtg_json_query_result_t {
    /// Json body of the query, this is NULL on error
    json_t *json;
} mtg_json_query_result_t;

/// Returns 1 on success, 0 on failure
int get_atomic_cards(mtg_json_query_result_t *ret, thread_pool_t *pool);

