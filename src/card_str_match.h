#pragma once
#include <regex.h>
#include "./avl_tree.h"
#include "./card.h"
#include "./thread_pool.h"

#define MSE_RE_FLAGS (REG_EXTENDED | REG_ICASE)

/// Helper to use the correct flags for the regex, use regfree on your regex afterwards
int mse_compile_regex(char *regex, regex_t *re);

/// Returns 1 if there are one or matches in the oracle for the regex
int mse_card_oracle_matches(mtg_card_t *card, regex_t *re);

/// Returns 1 if there are one or matches in the name for the regex
int mse_card_name_matches(mtg_card_t *card, regex_t *re);

int mse_matching_card_oracle(avl_tree_node_t **ret,
                             avl_tree_node_t *cards_tree,
                             char *regex,
                             thread_pool_t *pool);

int mse_matching_card_name(avl_tree_node_t **ret,
                           avl_tree_node_t *cards_tree,
                           char *regex,
                           thread_pool_t *pool);
