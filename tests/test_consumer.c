#include "../testing_h/testing.h"
#include "../mse/consumer.h"
#include "./test_generators.h"
#include <string.h>

#define ARGUMENT "Lorem ipsum dolor sit amet, qui minim labore adipisicing minim sint cillum sint consectetur cupidatat."

static int test_consumer_init_free()
{
    mse_set_consumer_t consumer;
    ASSERT(mse_init_set_consumer(&consumer,
                                 MSE_SET_CONSUMER_NEGATE,
                                 "",
                                 0));
    ASSERT(consumer.generator_type == MSE_SET_CONSUMER_NEGATE);
    ASSERT(consumer.argument != NULL);
    ASSERT(strlen(consumer.argument) == 0);
    mse_free_set_consumer(&consumer);

    ASSERT(mse_init_set_consumer(&consumer,
                                 MSE_SET_CONSUMER_ORACLE,
                                 ARGUMENT,
                                 strlen(ARGUMENT)));
    ASSERT(consumer.generator_type == MSE_SET_CONSUMER_ORACLE);
    ASSERT(consumer.argument != NULL);
    ASSERT(strcmp(consumer.argument, ARGUMENT) == 0);
    mse_free_set_consumer(&consumer);

    ASSERT(mse_init_set_consumer(&consumer,
                                 MSE_SET_CONSUMER_NAME,
                                 ARGUMENT,
                                 strlen(ARGUMENT)));
    ASSERT(consumer.generator_type == MSE_SET_CONSUMER_NAME);
    ASSERT(consumer.argument != NULL);
    ASSERT(strcmp(consumer.argument, ARGUMENT) == 0);
    mse_free_set_consumer(&consumer);
    return 1;
}

static int test_consumer_negate_no_cards()
{
    mse_set_consumer_t consumer;
    ASSERT(mse_init_set_consumer(&consumer,
                                 MSE_SET_CONSUMER_NEGATE,
                                 "",
                                 0));
    ASSERT(consumer.generator_type == MSE_SET_CONSUMER_NEGATE);
    ASSERT(consumer.argument != NULL);
    ASSERT(strlen(consumer.argument) == 0);
    mse_free_set_consumer(&consumer);

    mse_avl_tree_node_t *root = NULL;
    mse_search_intermediate_t ret, child;
    memset(&child, 0, sizeof(child));
    child.node = root;

    ASSERT(mse_consume_set(&consumer, &ret, &gen_cards, &child, &gen_thread_pool));
    ASSERT(mse_tree_size(ret.node) == mse_tree_size(gen_cards.card_tree));

    mse_free_search_intermediate(&ret);
    return 1;
}

SUB_TEST(test_consumer, {&test_consumer_init_free, "Test consumer init and free"},
{&test_consumer_negate_no_cards, "Test consumer negate"})
