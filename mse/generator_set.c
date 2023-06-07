#include "./generator_set.h"
#include "./uuid.h"
#include "./search.h"
#include "./set.h"
#include "../testing_h/testing.h"
#include <string.h>

int mse_generate_set_set(mse_set_generator_t *gen,
                         mse_search_intermediate_t *res,
                         mse_all_printings_cards_t *cards)
{
    mse_set_t card_set_proxy;
    ASSERT(mse_get_set_code(gen->argument, &card_set_proxy.code));

    mse_avl_tree_node_t *node = mse_find_payload(cards->set_tree, (void *) &card_set_proxy);
    if (node == NULL) {
        return 1;
    }

    mse_set_t *set = node->payload;
    *res = mse_init_search_intermediate_tree(set->set_cards_tree, 1);

    if (gen->negate) {
        mse_search_intermediate_t tmp;
        memset(&tmp, 0, sizeof(tmp));

        ASSERT(mse_set_negate(&tmp, cards, res));
        mse_free_search_intermediate(res);
        *res = tmp;
    }
    return 1;
}
