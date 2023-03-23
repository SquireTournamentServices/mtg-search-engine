#pragma once
#include "./avl_tree.h"

typedef enum mse_search_intermediate_type_t {
    MSE_INTERMEDIATE_ERR,
    MSE_INTERMEDIATE_TREE,
    MSE_INTERMEDIATE_LIST
} mse_search_intermediate_type_t;

typedef struct mse_search_intermediate_t {
    mse_search_intermediate_type_t type;
    union {
        avl_tree_node *node;
        avl_tree_lookup_t list;
    };
} mse_search_intermediate_t;

mse_search_intermediate_t init_mse_search_intermediate_tree(avl_tree_node *node);

/// Set union for two intermediates
int mse_set_union(mse_search_intermediate_t *ret,
                  mse_search_intermediate_t *a,
                  mse_search_intermediate_t *b);

/// Set intersection for two intermediates
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
    avl_tree_node *cards;
    size_t cards_count;
    mse_search_sort_type_t current_sort;
} mse_search_result_t;
