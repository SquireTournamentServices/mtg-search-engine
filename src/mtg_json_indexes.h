#pragma once
#include <semaphore.h>
#include "./mtg_json.h"

/// Internal state for generating indexes
/// Used by the implementation for int __generate_indexes(mtg_all_printings_cards_t *ret, thread_pool_t *pool);
/// Which is in this unit

typedef struct mse_index_generator_state_t {
    mtg_all_printings_cards_t *cards; // owned by the caller
    thread_pool_t *pool; // owned by the caller
    sem_t semaphore;
    int ret;
} mse_index_generator_state_t;

void free_index_generator_state(mse_index_generator_state_t *state);

// __generate_indexes is defined in the mtg_json.h header, but declared in this unit
