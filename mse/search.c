#include "./search.h"
#include "../testing_h/testing.h"
#include "./card.h"
#include <string.h>

static void __mse_init_search_intermediate(mse_search_intermediate_t *inter)
{
    memset(inter, 0, sizeof(*inter));
}

mse_search_intermediate_t mse_init_search_intermediate_tree(mse_avl_tree_node_t *node, int is_reference)
{
    mse_search_intermediate_t ret;
    __mse_init_search_intermediate(&ret);
    ret.node = node;
    ret.is_reference = is_reference;
    return ret;
}

static int __copy_node(mse_avl_tree_node_t **node, mse_avl_tree_node_t *node_old)
{
    *node = mse_shallow_copy_tree_node(node_old);
    ASSERT(*node != NULL);
    (*node)->cmp_payload = MSE_CARD_DEFAULT_COMPARE_FUNCTION;
    (*node)->free_payload = MSE_CARD_DEFAULT_FREE_FUNCTION;
    return 1;
}

static int __insert_tree_set_union(mse_avl_tree_node_t **tree, mse_avl_tree_node_t *node_old)
{
    if (node_old == NULL) {
        return 1;
    }

    mse_avl_tree_node_t *node = NULL;
    ASSERT(__copy_node(&node, node_old));

    if (!mse_insert_node(tree, node)) {
        mse_free_tree(node);
    }

    if (!__insert_tree_set_union(tree, node_old->l)) {
        mse_free_tree(node);
        return 0;
    }

    if (!__insert_tree_set_union(tree, node_old->r)) {
        mse_free_tree(node);
        return 0;
    }
    return 1;
}

int mse_set_union(mse_search_intermediate_t *ret,
                  mse_search_intermediate_t *a,
                  mse_search_intermediate_t *b)
{
    __mse_init_search_intermediate(ret);
    ASSERT(__insert_tree_set_union(&ret->node, a->node));
    ASSERT(__insert_tree_set_union(&ret->node, b->node));
    return 1;
}

static int __mse_set_intersection(mse_avl_tree_node_t **ret,
                                  mse_avl_tree_node_t *node,
                                  mse_avl_tree_node_t *tree)
{
    if (node == NULL) {
        return 1;
    }

    if (mse_find_payload(tree, node->payload)) {
        mse_avl_tree_node_t *node_copy = NULL;
        ASSERT(__copy_node(&node_copy, node));
        ASSERT(mse_insert_node(ret, node_copy));
    }

    ASSERT(__mse_set_intersection(ret, node->l, tree));
    ASSERT(__mse_set_intersection(ret, node->r, tree));
    return 1;
}

int mse_set_intersection(mse_search_intermediate_t *ret,
                         mse_search_intermediate_t *a,
                         mse_search_intermediate_t *b)
{
    __mse_init_search_intermediate(ret);
    if (a->node == NULL || b->node == NULL) {
        return 1;
    }

    // Small performance increase as the intersection by iterating over the smaller set then finding each in the
    // other is far faster
    if (a->node->height > b->node->height) {
        return mse_set_intersection(ret, b, a);
    }

    return __mse_set_intersection(&ret->node, a->node, b->node);
}

static int __mse_set_negate(mse_search_intermediate_t *ret,
                            mse_avl_tree_node_t *node,
                            mse_search_intermediate_t *a)
{
    // Base case
    if (node == NULL) {
        return 1;
    }

    // If node is not in a then add it to ret
    if (!mse_find_payload(a->node, node->payload)) {
        mse_avl_tree_node_t *copy_node = NULL;
        ASSERT(__copy_node(&copy_node, node));
        ASSERT(mse_insert_node(&ret->node, copy_node));
    }

    // Recurse
    ASSERT(__mse_set_negate(ret, node->l, a));
    ASSERT(__mse_set_negate(ret, node->r, a));
    return 1;
}

int mse_set_negate(mse_search_intermediate_t *ret,
                   mse_all_printings_cards_t *cards,
                   mse_search_intermediate_t *a)
{
    __mse_init_search_intermediate(ret);
    return __mse_set_negate(ret, cards->card_tree, a);
}

void mse_free_search_intermediate(mse_search_intermediate_t *inter)
{
    if (inter->node != NULL && !inter->is_reference) {
        mse_free_tree(inter->node);
    }
    memset(inter, 0, sizeof(*inter));
}

/// In order insert of the cards tree into the output array
static int __mse_finalise_search_insert(mse_search_result_t *search_final_res,
                                        mse_avl_tree_node_t *node,
                                        size_t insert_index)
{
    if (node == NULL) {
        return 0;
    }

    insert_index += __mse_finalise_search_insert(search_final_res, node->l, insert_index);
    search_final_res->cards[insert_index++] = (mse_card_t *) node->payload;
    insert_index += __mse_finalise_search_insert(search_final_res, node->r, insert_index);
    return insert_index;
}

int mse_finalise_search(mse_search_result_t *search_final_res, mse_search_intermediate_t *search_int_res)
{
    memset(search_final_res, 0, sizeof(*search_final_res));
    search_final_res->current_sort = MSE_SORT_DEFAULT;
    search_final_res->cards_length = mse_tree_size(search_int_res->node);
    ASSERT(search_final_res->cards = malloc(sizeof(*search_final_res->cards) * search_final_res->cards_length));

    size_t cards_real_length = __mse_finalise_search_insert(search_final_res, search_int_res->node, 0);
    mse_free_search_intermediate(search_int_res);

    // Sanity check the length
    ASSERT(cards_real_length == search_final_res->cards_length);
    return 1;
}

void mes_sort_search_results(mse_search_result_t *search_res, mse_search_sort_type_t sort_type)
{
    // Already sorted this way, no need to recompute
    if (search_res->current_sort == sort_type) {
        return;
    }

    search_res->current_sort = sort_type;

    int (*sort_fn)(void *, void *) = NULL;
    switch(search_res->current_sort) {
    case MSE_SORT_CARD_NAME:
        sort_fn = &mse_avl_cmp_card_name;
        break;
    case MSE_SORT_CMC:
        sort_fn = &mse_avl_cmp_card_cmc;
        break;
    case MSE_SORT_UUID:
        sort_fn = &mse_avl_cmp_card;
        break;
    case MSE_SORT_POWER:
        sort_fn = &mse_avl_cmp_card_power;
        break;
    case MSE_SORT_TOUGHNESS:
        sort_fn = &mse_avl_cmp_card_toughness;
        break;
    }
    qsort(search_res->cards,
          search_res->cards_length,
          sizeof(*search_res->cards),
          // Sketchy looking cast lmao
          (int (*)(const void *, const void *)) sort_fn);
}

void mse_free_search_results(mse_search_result_t *search_res)
{
    if (search_res->cards != NULL) {
        free(search_res->cards);
    }
    memset(search_res, 0, sizeof(*search_res));
}
