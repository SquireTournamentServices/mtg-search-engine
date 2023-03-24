#include "./search.h"
#include "../testing_h/testing.h"
#include "./card.h"
#include <string.h>

// As this is a library to search cards all the trees that this uses are trees whose payloads are cards.
#define COMPARE_FUNCTION &avl_cmp_card
#define FREE_FUNCTION NULL

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

static int __insert_tree_set_union(avl_tree_node_t **tree, avl_tree_node_t *node_old)
{
    if (node_old == NULL) {
        return 1;
    }

    // Shallow copy of the node
    avl_tree_node_t *node = shallow_copy_tree_node(node_old);
    ASSERT(node != NULL);
    node->cmp_payload = COMPARE_FUNCTION;
    node->free_payload = FREE_FUNCTION;

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

int mse_set_intersection(mse_search_intermediate_t *ret,
                         mse_search_intermediate_t *a,
                         mse_search_intermediate_t *b)
{
    __init_mse_search_intermediate(ret);
    return 0;
}

void free_mse_search_intermediate(mse_search_intermediate_t *inter)
{
    if (inter->node != NULL) {
        free_tree(inter->node);
    }
    memset(inter, 0, sizeof(*inter));
}
