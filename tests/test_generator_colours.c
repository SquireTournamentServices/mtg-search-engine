#include "./test_generators.h"
#include "../src/card.h"
#include "../src/generators.h"
#include "../src/avl_tree.h"
#include "../testing_h/testing.h"
#include <string.h>

#define DEFAULT_ARGUMENT (MSE_WHITE | MSE_BLUE)
#define DEFAULT_ARGUMENT_STR "WU"

static int test_tree(avl_tree_node_t *node)
{
    if (node == NULL) {
        return 1;
    }

    mse_card_t *card = (mse_card_t *) node->payload;
    ASSERT(card->colours == DEFAULT_ARGUMENT);

    ASSERT(test_tree(node->l));
    ASSERT(test_tree(node->r));
    return 1;
}

int test_generator_colours()
{
    mse_set_generator_type_t gen_type = MSE_SET_GENERATOR_COLOUR;
    size_t len = strlen(DEFAULT_ARGUMENT_STR);

    mse_set_generator_t ret;
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_EQUALS, DEFAULT_ARGUMENT_STR, len));

    avl_tree_node_t *root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree(root));
    free_tree(root);
    mse_free_set_generator(&ret);

    // Test includes
    ASSERT(mse_init_set_generator(&ret, gen_type, MSE_SET_GENERATOR_OP_INCLUDES, DEFAULT_ARGUMENT_STR, len));
    root = NULL;
    ASSERT(mse_generate_set(&ret, &root, &gen_cards, &gen_thread_pool));
    ASSERT(tree_size(root) > 0);
    ASSERT(test_tree(root));
    free_tree(root);
    mse_free_set_generator(&ret);
    return 1;
}
