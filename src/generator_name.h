#pragma once
#include "./generators.h"
#include "./avl_tree.h"
#include "./thread_pool.h"
#include "./mtg_json.h"

int mse_generate_set_name(mse_set_generator_t *gen,
                          avl_tree_node_t **res,
                          mse_all_printings_cards_t *cards,
                          thread_pool_t *pool);
