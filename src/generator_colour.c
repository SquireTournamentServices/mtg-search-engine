#include "./generator_colour.h"
#include "./card.h"
#include "./search.h"
#include "../testing_h/testing.h"
#include <string.h>

static int __mse_parse_colours(char *colours, mse_colour_enum_t *ret)
{
    *ret = 0;
    for (size_t i = 0; colours[i] != 0; i++) {
        int colour;
        ASSERT(colour = mse_parse_colour(colours[i]));
        *ret |= colour;
    }
    ASSERT(*ret <= MSE_WUBRG);
    return 1;
}

int mse_generate_set_colours(mse_set_generator_t *gen,
                             mse_search_intermediate_t *res,
                             mse_all_printings_cards_t *cards)
{
    mse_colour_enum_t colours = 0;
    ASSERT(__mse_parse_colours(gen->argument, &colours));

    // Get the data
    avl_tree_node_t *node =  NULL;
    switch(gen->generator_op) {
    case MSE_SET_GENERATOR_OP_LT:
        node = cards->indexes.colour_index.colours_lt.colour_indexes[colours];
        break;
    case MSE_SET_GENERATOR_OP_LT_INC:
        node = cards->indexes.colour_index.colours_lt_inc.colour_indexes[colours];
        break;
    case MSE_SET_GENERATOR_OP_GT:
        node = cards->indexes.colour_index.colours_gt.colour_indexes[colours];
        break;
    case MSE_SET_GENERATOR_OP_GT_INC:
        node = cards->indexes.colour_index.colours_gt_inc.colour_indexes[colours];
        break;
    case MSE_SET_GENERATOR_OP_EQUALS:
    case MSE_SET_GENERATOR_OP_INCLUDES:
        node = cards->indexes.colour_index.colours_eq.colour_indexes[colours];
        break;
    }

    ASSERT(node != NULL);
    *res = init_mse_search_intermediate_tree(node, 1);
    return 1;
}

int mse_generate_set_colour_identity(mse_set_generator_t *gen,
                                     mse_search_intermediate_t *res,
                                     mse_all_printings_cards_t *cards)
{
    mse_colour_enum_t colours = 0;
    ASSERT(__mse_parse_colours(gen->argument, &colours));

    // Get the data
    avl_tree_node_t *node =  NULL;
    switch(gen->generator_op) {
    case MSE_SET_GENERATOR_OP_LT:
        node = cards->indexes.colour_index.colour_identity_lt.colour_indexes[colours];
        break;
    case MSE_SET_GENERATOR_OP_LT_INC:
        node = cards->indexes.colour_index.colour_identity_lt_inc.colour_indexes[colours];
        break;
    case MSE_SET_GENERATOR_OP_GT:
        node = cards->indexes.colour_index.colour_identity_gt.colour_indexes[colours];
        break;
    case MSE_SET_GENERATOR_OP_GT_INC:
        node = cards->indexes.colour_index.colour_identity_gt_inc.colour_indexes[colours];
        break;
    case MSE_SET_GENERATOR_OP_EQUALS:
    case MSE_SET_GENERATOR_OP_INCLUDES:
        node = cards->indexes.colour_index.colour_identity_eq.colour_indexes[colours];
        break;
    }

    ASSERT(node != NULL);
    *res = init_mse_search_intermediate_tree(node, 1);
    return 1;
}
