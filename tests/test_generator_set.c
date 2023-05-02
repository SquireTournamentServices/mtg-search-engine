#include "./test_generators.h"
#include "../testing_h/testing.h"
#include "../src/card.h"
#include "../src/avl_tree.h"
#include "../src/generators.h"
#include <string.h>

#define DEFAULT_ARGUMENT "m20"

static int test_tree(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    mse_card_t *card = (mse_card_t *) node->payload;
    int found = 0;
    for (size_t i = 0; i < card->set_codes_count; i++) {
        found |= memcmp(card->set_codes[i], DEFAULT_ARGUMENT, 3) == 0;
        if (found) {
            break;
        }
    }
    ASSERT(found);

    ASSERT(test_tree(node->l));
    ASSERT(test_tree(node->r));
    return 1;
}

int test_generator_set()
{
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_SET;
    size_t len = strlen(DEFAULT_ARGUMENT);

    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_EQUALS, DEFAULT_ARGUMENT, len));

    avl_tree_node_t *root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree(root));
    free_tree(root);
    mse_free_set_generator(&ret);

    // Test includes
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_INCLUDES, DEFAULT_ARGUMENT, len));
    root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree(root));
    free_tree(root);
    mse_free_set_generator(&ret);
    return 1;
}
