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

/// This will store the atomic cards and, sets. Each card is stored once and,
/// only once. cards and, sets are both stored as binary trees sorted by UUIDs.
/// other indices need to be generated afterwards.
typedef struct mtg_all_printings_cards_t {
    /// Amount of sets
    size_t set_count;
    /// AVL BST tree for the sets, sorted by set code
    avl_tree_node *set_tree;

    /// Amount of cards
    size_t card_count;
    /// AVL BST tree for the cards, sorted by UUID
    avl_tree_node *card_tree;
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

/// Frees atomic cards
void free_all_printings_cards(mtg_all_printings_cards_t *cards);

/// This will store all cards  and, their indexes
typedef struct mtg_indexed_cards_t {
    mtg_all_printings_cards_t all_printings_cards;

} mtg_indexed_cards_t;

