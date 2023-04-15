#pragma once
#include <regex.h>
#include "./avl_tree.h"
#include "./card.h"
#include "./thread_pool.h"

#define MSE_RE_FLAGS (REG_EXTENDED | REG_ICASE)

/// Returns whether or no a string is a regex string
/// /abc/ is a regex string, otherwise it would be a normal string
int mse_is_regex_str(char *str);

/// Helper to use the correct flags for the regex, use regfree on your regex afterwards
int mse_compile_regex(char *regex, regex_t *re);

/// Returns 1 if there are one or matches in the oracle for the regex
int mse_card_oracle_matches(mtg_card_t *card, regex_t *re);

/// Returns 1 if there are one or matches in the name for the regex
int mse_card_name_matches(mtg_card_t *card, regex_t *re);

/// Returns a set of matching cards for an oracle regex
int mse_matching_card_oracle(avl_tree_node_t **ret,
                             avl_tree_node_t *cards_tree,
                             char *regex,
                             thread_pool_t *pool);

/// Returns a set of matching cards for a name regex
int mse_matching_card_name(avl_tree_node_t **ret,
                           avl_tree_node_t *cards_tree,
                           char *regex,
                           thread_pool_t *pool);

/// The syntax for the langugage wraps regex in slashes, these will be removed by calling this
/// Takes a regex string i.e:
/// abc\\/bcdef -> abc\/bcdef
/// /test123/ -> test123
char *escape_regex(char *regex);
