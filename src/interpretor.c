#include "./interpretor.h"
#include "../testing_h/testing.h"
#include <stdlib.h>
#include <string.h>

static mse_interp_node_t *__mse_init_interp_node()
{
    mse_interp_node_t *ret = malloc(sizeof(*ret));
    ASSERT(ret != NULL);
    memset(ret, 0, sizeof(*ret));
    return ret;
}

mse_interp_node_t *mse_init_interp_node_operation(mse_set_operator_type_t operation)
{
    mse_interp_node_t *ret = __mse_init_interp_node();
    ASSERT(ret != NULL);

    ret->type = MSE_INTERP_NODE_SET_OPERATOR;
    ret->op_type = operation;
    return ret;
}

mse_interp_node_t *mse_init_interp_node_generator(mse_set_generator_t generator)
{
    mse_interp_node_t *ret = __mse_init_interp_node();
    ASSERT(ret != NULL);

    ret->type = MSE_INTERP_NODE_SET_GENERATOR;
    ret->generator = generator;
    return ret;
}

void mse_free_interp_node(mse_interp_node_t *node)
{
    if (node == NULL) {
        return;
    }

    switch(node->type) {
    case MSE_INTERP_NODE_SET_OPERATOR:
        break;
    case MSE_INTERP_NODE_SET_GENERATOR:
        mse_free_set_generator(&node->generator);
        break;
    }

    mse_free_interp_node(node->l);
    mse_free_interp_node(node->r);
    free(node);
}

static int __mse_resolve_interp_leaf_generator(mse_interp_node_t *node,
        mse_search_intermediate_t *ret,
        thread_pool_t *pool,
        int dry_run,
        mse_all_printings_cards_t *cards)
{
    ASSERT(node->l == NULL);
    ASSERT(node->r == NULL);
    if (dry_run) {
        return 1;
    }

    ASSERT(mse_generate_set(&node->generator,
                            &ret->node,
                            cards,
                            pool));
    return 1;
}

static int __mse_resolve_interp_tree_operator(mse_interp_node_t *node,
        mse_search_intermediate_t *ret,
        thread_pool_t *pool,
        int dry_run,
        mse_all_printings_cards_t *cards)
{
    // Validate the node
    ASSERT(node->l != NULL);
    ASSERT(node->r != NULL);

    // Calculate sets a and b (this is the recursive step)
    mse_search_intermediate_t a, b;
    memset(&a, 0, sizeof(a));
    b = a;

    ASSERT(mse_resolve_interp_tree(node->l, &a, pool, dry_run, cards));
    ASSERT(mse_resolve_interp_tree(node->r, &b, pool, dry_run, cards));
    if (dry_run) {
        return 1;
    }

    // Perform set operation
    int r = 0;
    switch(node->op_type) {
    case MSE_SET_UNION:
        r = mse_set_union(ret, &a, &b);
        break;
    case MSE_SET_INTERSECTION:
        r = mse_set_intersection(ret, &a, &b);
        break;
    }

    // Cleanup
    free_mse_search_intermediate(&a);
    free_mse_search_intermediate(&b);
    ASSERT(r);
    return 1;
}

int mse_resolve_interp_tree(mse_interp_node_t *root,
                            mse_search_intermediate_t *ret,
                            thread_pool_t *pool,
                            int dry_run,
                            mse_all_printings_cards_t *cards)
{
    switch(root->type) {
    case MSE_INTERP_NODE_SET_GENERATOR:
        ASSERT(__mse_resolve_interp_leaf_generator(root, ret, pool, dry_run, cards));
        break;
    case MSE_INTERP_NODE_SET_OPERATOR:
        ASSERT(__mse_resolve_interp_tree_operator(root, ret, pool, dry_run, cards));
        break;
    default:
        lprintf(LOG_ERROR, "Undefined node type\n");
        return 0;
    }
    return 1;
}
