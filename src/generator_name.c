#include "./generator_name.h"
#include "./card_str_match.h"
#include "./card_txt_fields_trie.h"
#include "./search.h"
#include "../testing_h/testing.h"
#include <string.h>

static int __mse_generate_set_name_re(mse_set_generator_t *gen,
                                      avl_tree_node_t **res,
                                      mtg_all_printings_cards_t *cards,
                                      thread_pool_t *pool)
{
    char *re = escape_regex(gen->argument);
    ASSERT(re != NULL);

    int status = mse_matching_card_name(res, cards->card_tree, re, pool);
    free(re);

    ASSERT(status);
    return 1;
}

static int __mse_generate_set_name_text_inc(mse_set_generator_t *gen,
        avl_tree_node_t **res,
        mtg_all_printings_cards_t *cards,
        thread_pool_t *pool)
{
    // Split the name into pieces
    mse_card_name_parts_t parts;
    ASSERT(mse_split_card_name(gen->argument, &parts));

    // Calculate the sets for each part
    mse_search_intermediate_t search_tmp;
    memset(&search_tmp, 0, sizeof(search_tmp));

    // Calculate the set intersection of all of the parts
    int code = 1;
    for (size_t i = 0; i < parts.len; i++) {
        mse_search_intermediate_t tmp, tmp_trie;
        memset(&tmp, 0, sizeof(tmp));
        tmp_trie = tmp;

        int r = mse_card_trie_lookup_aprox(cards->indexes.card_name_parts_trie, parts.parts[i], &tmp_trie.node);
        if (!r) {
            lprintf(LOG_ERROR, "Cannot lookup the trie\n");
            code = 0;
            break;
        }

        r = mse_set_union(&tmp, &search_tmp, &tmp_trie);

        // Free before the error check
        free_mse_search_intermediate(&tmp_trie);
        if (!r) {
            lprintf(LOG_ERROR, "Cannot generate set union\n");
            code = 0;
            break;
        }

        free_mse_search_intermediate(&search_tmp);
        search_tmp = tmp;
    }

    // Cleanup
    free_mse_card_parts(&parts);
    if (!code) {
        free_mse_search_intermediate(&search_tmp);
        lprintf(LOG_ERROR, "Cannot generate set for name text inc\n");
        return 0;
    }

    *res = search_tmp.node;
    return 1;
}

int mse_generate_set_name(mse_set_generator_t *gen,
                          avl_tree_node_t **res,
                          mtg_all_printings_cards_t *cards,
                          thread_pool_t *pool)
{
    if (mse_is_regex_str(gen->argument)) {
        return __mse_generate_set_name_re(gen, res, cards, pool);
    } else {
        return __mse_generate_set_name_text_inc(gen, res, cards, pool);
    }
}
