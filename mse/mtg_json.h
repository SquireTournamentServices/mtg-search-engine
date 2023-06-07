#pragma once
#include "./thread_pool.h"
#include "./card.h"
#include "./set.h"
#include "./avl_tree.h"
#include "./card_txt_fields_trie.h"
#include <jansson.h>

#define ATOMIC_CARDS_URL "https://mtgjson.com/api/v5/AllPrintings.json"

/// Exposed internal method for use within internal testing
size_t __mse_json_write_callback(char *ptr,
                                 size_t size,
                                 size_t nmemb,
                                 void *data);

#define MSE_COLOUR_INDEX_MAX_CARDS (MSE_WUBRG + 1)

/// The indexes for the cards which are in a large list and sorted by
/// the bit flags from 00000 to 11111
typedef struct mse_colour_index_t {
    avl_tree_node_t * colour_indexes[MSE_COLOUR_INDEX_MAX_CARDS];
} mse_colour_index_t;

void mse_free_colour_index(mse_colour_index_t *index);

#define MSE_COLOUR_INDEX_GENERATOR(index_base_name) \
mse_colour_index_t index_base_name##_lt; \
mse_colour_index_t index_base_name##_lt_inc; \
mse_colour_index_t index_base_name##_gt; \
mse_colour_index_t index_base_name##_gt_inc; \
mse_colour_index_t index_base_name##_eq;

/// Holds all of the colour indexes
typedef struct mse_cards_colours_indexes_t {
    MSE_COLOUR_INDEX_GENERATOR(colours);
    MSE_COLOUR_INDEX_GENERATOR(colour_identity);
} mse_cards_colours_indexes_t;

/// Contains all of the card indexes, each index will have a payload that is a pointer to a card,
/// however it will have a NULL free pointer as the memory is owneed by mse_all_printings_cards_t
/// in card_tree
typedef struct mse_cards_indexes_t {
    /// Index for power
    avl_tree_node_t *card_power_tree;
    /// Index for toughness
    avl_tree_node_t *card_toughness_tree;
    /// Index for card cmc
    avl_tree_node_t *card_cmc_tree;
    /// Card names trie where the full filtered name is used
    mse_card_trie_node_t *card_name_trie;
    /// Card names parts trie where the full filtered name is used
    mse_card_trie_node_t *card_name_parts_trie;

    /// Colours index, this is a long list of stuff
    mse_cards_colours_indexes_t colour_index;
} mse_cards_indexes_t;

/// This will store the atomic cards and, sets. Each card is stored once and,
/// only once. cards and, sets are both stored as binary trees sorted by UUIDs.
/// other indices need to be generated afterwards.
typedef struct mse_all_printings_cards_t {
    /// Amount of sets
    size_t set_count;
    /// AVL BST tree for the sets, sorted by set code
    avl_tree_node_t *set_tree;

    /// Amount of cards
    size_t card_count;
    /// AVL BST tree for the cards, sorted by UUID
    avl_tree_node_t *card_tree;

    /// Indexes
    mse_cards_indexes_t indexes;
} mse_all_printings_cards_t;

/// Exposed internal method for use within internal testing
/// This function will handle a set node and add the set and, cards
int __mse_handle_all_printings_cards_set(mse_all_printings_cards_t *ret,
        const char *set_code,
        json_t *set_node);

/// Exposed internal method for use within internal testing
/// This method will parse the json cards all of the indexes
int __mse_parse_all_printings_cards(mse_all_printings_cards_t *ret, json_t *cards, thread_pool_t *pool);

/// Returns 1 on success, 0 on failure
/// Do not call mse_free_all_printings_cards on fail
int mse_get_all_printings_cards(mse_all_printings_cards_t *ret, thread_pool_t *pool);

/// This function will generate all of the indexes for a card, defined in mse_json_indexes.c
int __mse_generate_indexes(mse_all_printings_cards_t *ret, thread_pool_t *pool);

/// Frees atomic cards
void mse_free_all_printings_cards(mse_all_printings_cards_t *cards);

/// This will store all cards  and, their indexes
typedef struct mse_indexed_cards_t {
    mse_all_printings_cards_t all_printings_cards;

} mse_indexed_cards_t;
