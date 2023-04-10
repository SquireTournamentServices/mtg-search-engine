#include "./generator_oracle.h"
#include "./card_str_match.h"
#include "../testing_h/testing.h"
#include <string.h>

static int is_regex_str(char *str)
{
    size_t len = strlen(str);
    if (len < 2) {
        return 0;
    }
    return str[0] == '/' && str[len - 1] == '/';
}

int mse_generate_set_oracle_re(mse_set_generator_t *gen,
                               avl_tree_node_t **res,
                               mtg_all_printings_cards_t *cards,
                               thread_pool_t *pool)
{
    char *re = escape_regex(gen->argument);
    ASSERT(re != NULL);

    int status = mse_matching_card_oracle(res, cards->card_tree, re, pool);
    free(re);

    ASSERT(status);
    return 1;
}

int mse_generate_set_oracle_text_inc(mse_set_generator_t *gen,
                                     avl_tree_node_t **res,
                                     mtg_all_printings_cards_t *cards,
                                     thread_pool_t *pool)
{
    return 0;
}

int mse_generate_set_oracle(mse_set_generator_t *gen,
                            avl_tree_node_t **res,
                            mtg_all_printings_cards_t *cards,
                            thread_pool_t *pool)
{
    if (is_regex_str(gen->argument)) {
        return mse_generate_set_oracle_re(gen, res, cards, pool);
    } else {
        return mse_generate_set_oracle_text_inc(gen, res, cards, pool);
    }
}
