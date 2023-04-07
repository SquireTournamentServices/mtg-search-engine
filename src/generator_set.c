#include "./generator_set.h"
#include "./uuid.h"
#include "./search.h"
#include "./set.h"
#include "../testing_h/testing.h"
#include <string.h>

int mse_generate_set_set(mse_set_generator_t *gen,
                         avl_tree_node_t **res,
                         mtg_all_printings_cards_t *cards)
{
    *res = NULL;
    mtg_set_t card_set_proxy;
    ASSERT(get_set_code(gen->argument, &card_set_proxy.code));

    avl_tree_node_t *node = find_payload(cards->set_tree, (void *) &card_set_proxy);
    if (node == NULL) {
        return 1;
    }

    mtg_set_t *set = node->payload;

    // Shallow copy of the set_cards_tree
    mse_search_intermediate_t ret, a;
    memset(&ret, 0, sizeof(ret));
    a.node = set->set_cards_tree;

    ASSERT(mse_set_union(&ret, &a, NULL));
    *res = ret.node;
    return 1;
}
