#pragma once
#include "./avl_tree.h"

/// A wrapper for the types of data that set operations are defined on
typedef struct mse_search_intermediate_t {
    avl_tree_node_t *node;
} mse_search_intermediate_t;

/// This wraps a tree in a mse_search_intermediate_t for use within operations. There is no allocation here
/// often you want to free_tree(node) at some point down the line.
mse_search_intermediate_t init_mse_search_intermediate_tree(avl_tree_node_t *node);

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

void free_mse_search_intermediate(mse_search_intermediate_t *inter);

typedef enum mse_search_sort_type_t {
    MSE_SORT_CARD_NAME,
    MSE_SORT_CMC,
    MSE_SORT_UUID,
    MSE_SORT_POWER,
    MSE_SORT_TOUGHNESS
} mse_search_sort_type_t;

typedef struct mse_search_result_t {
    avl_tree_node_t *cards;
    size_t cards_count;
    mse_search_sort_type_t current_sort;
} mse_search_result_t;
