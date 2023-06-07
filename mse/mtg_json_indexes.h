#pragma once
#include <semaphore.h>
#include "./mtg_json.h"

/// Internal state for generating indexes
/// Used by the implementation for int __mse_generate_indexes(mse_all_printings_cards_t *ret, thread_pool_t *pool);
/// Which is in this unit

typedef struct mse_index_generator_state_t {
    mse_all_printings_cards_t *cards; // owned by the caller
    thread_pool_t *pool; // owned by the caller
    sem_t semaphore;
    int ret;
} mse_index_generator_state_t;

void mse_free_index_generator_state(mse_index_generator_state_t *state);

// __mse_generate_indexes is defined in the mse_json.h header, but declared in this unit
