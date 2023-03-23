#include "./search.h"
#include "../testing_h/testing.h"
#include <string.h>

mse_search_intermediate_t init_mse_search_intermediate_tree(avl_tree_node *node)
{
    mse_search_intermediate_t ret;
    ret.type = MSE_INTERMEDIATE_TREE;
    ret.node = node;
    return ret;
}

static void __init_mse_search_intermediate(mse_search_intermediate_t *inter)
{
    memset(inter, 0, sizeof(*inter));
}

static int __insert_tree_set_union(avl_tree_node **tree, avl_tree_node *node_old)
{
    if (node_old == NULL) {
        return 1;
    }

    // Shallow copy of the node
    avl_tree_node *node = shallow_copy_tree_node(node_old);
    ASSERT(node != NULL);

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

    ret->type = MSE_INTERMEDIATE_TREE;
    if (a->type == MSE_INTERMEDIATE_TREE) {
        ASSERT(__insert_tree_set_union(&ret->node, a->node));
    } else {
        //TODO: Insert all list elements for a
        return 0;
    }

    if (b->type == MSE_INTERMEDIATE_TREE) {
        ASSERT(__insert_tree_set_union(&ret->node, b->node));
    } else {
        // TODO: Insert all elements of list a then b
        return 0;
    }
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
    switch (inter->type) {
    case MSE_INTERMEDIATE_ERR:
        break;
    case MSE_INTERMEDIATE_TREE:
        free_tree(inter->node);
        break;
    case MSE_INTERMEDIATE_LIST:
        free_tree_lookup(&inter->list);
        break;
    }

    memset(inter, 0, sizeof(*inter));
}
