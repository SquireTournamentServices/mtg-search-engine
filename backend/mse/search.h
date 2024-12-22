#pragma once
#include "./avl_tree.h"
#include "./mtg_json.h"
#include "./card.h"

/// A wrapper for the types of data that set operations are defined on
typedef struct mse_search_intermediate_t {
    mse_avl_tree_node_t *node;
    int is_reference;
} mse_search_intermediate_t;

/// This wraps a tree in a mse_search_intermediate_t for use within operations. There is no allocation here
/// often you want to mse_free_tree(node) at some point down the line.
/// is_reference is whether the node is a reference to another objects memory or if it should be owned by this object
mse_search_intermediate_t mse_init_search_intermediate_tree(mse_avl_tree_node_t *node, int is_reference);

/// Enum for the operations defined here, used by interpretor.h
typedef enum mse_set_operator_type_t {
    MSE_SET_UNION,
    MSE_SET_INTERSECTION
} mse_set_operator_type_t;

/// Set union for two intermediates. Freeing this does free the payloads that a and b have, these payloads
/// are shared references and you have to free them yourself at some point.
int mse_set_union(mse_search_intermediate_t *ret,
                  mse_search_intermediate_t *a,
                  mse_search_intermediate_t *b);

/// Set intersection for two intermediates. Freeing this does free the payloads that a and b have, these
/// payloads are shared references and you have to free them yourself at some point.
int mse_set_intersection(mse_search_intermediate_t *ret,
                         mse_search_intermediate_t *a,
                         mse_search_intermediate_t *b);

/// Performs a set negation on the input state, this is far less efficient than performing a optimisation
/// to the set generator i.e: -name:"asdf" should check that the regex doesn't match, however for large
/// bracketed statements this is likely the only way to do it
///
/// TL;DR: try not to use this methodlmao
int mse_set_negate(mse_search_intermediate_t *ret,
                   mse_all_printings_cards_t *cards,
                   mse_search_intermediate_t *a);

/// Frees a search intermediate that has been initialised, call even on init fail.
void mse_free_search_intermediate(mse_search_intermediate_t *inter);

/// All supported sort types for a card in the card results
typedef enum mse_search_sort_type_t {
    MSE_SORT_CARD_NAME,
    MSE_SORT_CMC,
    MSE_SORT_UUID,
    MSE_SORT_POWER,
    MSE_SORT_TOUGHNESS
} mse_search_sort_type_t;

#define MSE_SORT_DEFAULT MSE_SORT_UUID

/// Stores the search results of the cards as an array and the current sort type
typedef struct mse_search_result_t {
    size_t cards_length;
    /// An array of card pointers, the card pointers are references to the master card tree
    mse_card_t **cards;
    mse_search_sort_type_t current_sort;
} mse_search_result_t;

/// Turns the search intermediate into an array with the MSE_SORT_DEFAULT sort type
/// The search intermediate is then freed and invalid.
int mse_finalise_search(mse_search_result_t *search_final_res, mse_search_intermediate_t *search_int_res);

/// Takes a search result and resorts it if the sort type has changed
void mse_sort_search_results(mse_search_result_t *search_res, mse_search_sort_type_t sort_type);

/// Frees the search result
void mse_free_search_results(mse_search_result_t *search_res);
