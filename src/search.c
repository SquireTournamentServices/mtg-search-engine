#include "./search.h"
#include "../testing_h/testing.h"
#include "./card.h"
#include <string.h>

static void __init_mse_search_intermediate(mse_search_intermediate_t *inter)
{
    memset(inter, 0, sizeof(*inter));
}

mse_search_intermediate_t init_mse_search_intermediate_tree(avl_tree_node_t *node)
{
    mse_search_intermediate_t ret;
    __init_mse_search_intermediate(&ret);
    ret.node = node;
    return ret;
}

static int __copy_node(avl_tree_node_t **node, avl_tree_node_t *node_old)
{
    *node = shallow_copy_tree_node(node_old);
    ASSERT(*node != NULL);
    (*node)->cmp_payload = MSE_CARD_DEFAULT_COMPARE_FUNCTION;
    (*node)->free_payload = MSE_CARD_DEFAULT_FREE_FUNCTION;
    return 1;
}

static int __insert_tree_set_union(avl_tree_node_t **tree, avl_tree_node_t *node_old)
{
    if (node_old == NULL) {
        return 1;
    }

    avl_tree_node_t *node = NULL;
    ASSERT(__copy_node(&node, node_old));

    if (!insert_node(tree, node)) {
        free_tree(node);
    }

    if (!__insert_tree_set_union(tree, node_old->l)) {
        free_tree(node);
        return 0;
    }

    if (!__insert_tree_set_union(tree, node_old->r)) {
        free_tree(node);
        return 0;
    }
    return 1;
}

int mse_set_union(mse_search_intermediate_t *ret,
                  mse_search_intermediate_t *a,
                  mse_search_intermediate_t *b)
{
    __init_mse_search_intermediate(ret);
    ASSERT(__insert_tree_set_union(&ret->node, a->node));
    ASSERT(__insert_tree_set_union(&ret->node, b->node));
    return 1;
}

static int __mse_set_intersection(avl_tree_node_t **ret,
                                  avl_tree_node_t *node,
                                  avl_tree_node_t *tree)
{
    if (node == NULL) {
        return 1;
    }

    if (find_payload(tree, node->payload)) {
        avl_tree_node_t *node_copy = NULL;
        ASSERT(__copy_node(&node_copy, node));
        ASSERT(insert_node(ret, node_copy));
    }

    ASSERT(__mse_set_intersection(ret, node->l, tree));
    ASSERT(__mse_set_intersection(ret, node->r, tree));
    return 1;
}

int mse_set_intersection(mse_search_intermediate_t *ret,
                         mse_search_intermediate_t *a,
                         mse_search_intermediate_t *b)
{
    __init_mse_search_intermediate(ret);
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

void free_mse_search_intermediate(mse_search_intermediate_t *inter)
{
    if (inter->node != NULL) {
        free_tree(inter->node);
    }
    memset(inter, 0, sizeof(*inter));
}
