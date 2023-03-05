#pragma once
#include "./thread_pool.h"
#include <jansson.h>

typedef struct mtg_json_query_result_t {
    /// Json body of the query, this is NULL on error
    json_t *json;
} mtg_json_query_result_t;

/// Returns 1 on success, 0 on failure
int get_atomic_cards(mtg_json_query_result_t *ret, thread_pool_t *pool);

