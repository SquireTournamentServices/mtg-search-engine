#pragma once
#include "./thread_pool.h"
#include "./card.h"
#include "./set.h"
#include "./avl_tree.h"
#include <jansson.h>

#define ATOMIC_CARDS_URL "https://mtgjson.com/api/v5/AllPrintings.json"

/// Exposed internal method for use within internal testing
size_t __mtg_json_write_callback(char *ptr,
                                 size_t size,
                                 size_t nmemb,
                                 void *data);

/// Contains all of the card indexes, each index will have a payload that is a pointer to a card,
/// however it will have a NULL free pointer as the memory is owneed by mtg_all_printings_cards_t
/// in card_tree
typedef struct mtg_cards_indexes {
    /// Index for power
    avl_tree_node_t *card_p_tree;
    /// Index for toughness
    avl_tree_node_t *card_t_tree;
    /// Index for card cmc
    avl_tree_node_t *card_cmc_tree;
} mtg_cards_indexes;

/// This will store the atomic cards and, sets. Each card is stored once and,
/// only once. cards and, sets are both stored as binary trees sorted by UUIDs.
/// other indices need to be generated afterwards.
typedef struct mtg_all_printings_cards_t {
    /// Amount of sets
    size_t set_count;
    /// AVL BST tree for the sets, sorted by set code
    avl_tree_node_t *set_tree;

    /// Amount of cards
    size_t card_count;
    /// AVL BST tree for the cards, sorted by UUID
    avl_tree_node_t *card_tree;

    /// Indexes
    mtg_cards_indexes indexes;
} mtg_all_printings_cards_t;

/// Exposed internal method for use within internal testing
/// This function will handle a set node and add the set and, cards
int __handle_all_printings_cards_set(mtg_all_printings_cards_t *ret,
                                     const char *set_code,
                                     json_t *set_node);

/// Exposed internal method for use within internal testing
/// This method will parse the json cards and create an index for set names
int __parse_all_printings_cards(mtg_all_printings_cards_t *ret, json_t *cards);

/// Returns 1 on success, 0 on failure
/// Do not call free_all_printings_cards on fail
int get_all_printings_cards(mtg_all_printings_cards_t *ret, thread_pool_t *pool);

/// This function will generate all of the indexes for a card, defined in mtg_json_indexes.c
int __generate_indexes(mtg_all_printings_cards_t *ret, thread_pool_t *pool);

/// Frees atomic cards
void free_all_printings_cards(mtg_all_printings_cards_t *cards);

/// This will store all cards  and, their indexes
typedef struct mtg_indexed_cards_t {
    mtg_all_printings_cards_t all_printings_cards;

} mtg_indexed_cards_t;

