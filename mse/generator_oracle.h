#pragma once
#include "./generators.h"
#include "./avl_tree.h"
#include "./thread_pool.h"
#include "./mtg_json.h"

extern int mse_generate_set_oracle(mse_set_generator_t *gen,
                                   mse_search_intermediate_t *res,
                                   mse_all_printings_cards_t *cards,
                                   mse_thread_pool_t *pool);
