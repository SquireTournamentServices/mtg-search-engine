#pragma once
#include <jansson.h>

typedef struct mtg_json_query_result_t {
    /// The return code, 200 is probably good
    int code;
    /// Json body of the query, this is NULL on error
    json_t *json;
} mtg_json_query_result_t;

mtg_json_query_result_t get_atomic_cards();

