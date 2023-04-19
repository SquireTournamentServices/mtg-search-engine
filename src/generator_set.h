#pragma once
#include "./generators.h"
#include "./avl_tree.h"
#include "./mtg_json.h"

int mse_generate_set_set(mse_set_generator_t *gen,
                         avl_tree_node_t **res,
                         mse_all_printings_cards_t *cards);
