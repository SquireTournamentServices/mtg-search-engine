#pragma once
#include "./mtg_json.h"
#include "./avl_tree.h"
#include "./generators.h"

int mse_generate_set_colours(mse_set_generator_t *gen,
                             avl_tree_node_t **res,
                             mse_all_printings_cards_t *cards);

int mse_generate_set_colour_identity(mse_set_generator_t *gen,
                                     avl_tree_node_t **res,
                                     mse_all_printings_cards_t *cards);
