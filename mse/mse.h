#pragma once
#include "./mtg_json.h"
#include "./thread_pool.h"
#include "./search.h"

/// The master state of the mse library
typedef struct mse_t {
    /// Cards and their indexes
    mse_all_printings_cards_t cards;
    /// Internal thread pool
    mse_thread_pool_t pool;
} mse_t;

/// Init he state of the mse library, WARNING: this will execute tzset
/// This includes: setting up the thread pool, downloading / reading the cards, and generating the indexes
extern int mse_init(mse_t *state);

/// Perform a search and save the result into res
extern int mse_search(mse_t *state, mse_search_result_t *res, const char *query);

/// Free the state of the mse library
extern void mse_free(mse_t *state);
