#pragma once
#include "./generators.h"
#include "./search.h"
#include "./thread_pool.h"
#include "./avl_tree.h"
#include "./mtg_json.h"

/// Type of the node
typedef enum mse_interp_node_type_t {
    /// A set generator
    MSE_INTERP_NODE_SET_GENERATOR,
    /// A set operation such as union or intersection
    MSE_INTERP_NODE_SET_OPERATOR,
} mse_interp_node_type_t;

/// A node for the interpretor tree
typedef struct mse_interp_node_t {
    /// Whether the union contains op_type (MSE_INTERP_NODE_SET_OPERATOR) or generator (MSE_INTERP_NODE_SET_GENERATOR)
    mse_interp_node_type_t type;
    union {
        mse_set_operator_type_t op_type;
        mse_set_generator_t generator;
    };
    struct mse_interp_node_t *l;
    struct mse_interp_node_t *r;
} mse_interp_node_t;

/// Inits an operation node with operation
mse_interp_node_t *mse_init_interp_node_operation(mse_set_operator_type_t operation);

/// Inits a set generator node with generator (this object is now owned by the tree)
mse_interp_node_t *mse_init_interp_node_generator(mse_set_generator_t generator);

/// Recursively frees the tree
void mse_free_interp_node(mse_interp_node_t *node);

/// Resolves a tree
/// if the dry_run flag is true
///   then it will not perform any calculations,
///   this is used to check parser output for validity and during tests
/// otherwise
///   this function will recurse over the tree to generate one output (ret)
int mse_resolve_interp_tree(mse_interp_node_t *root,
                            mse_search_intermediate_t *ret,
                            thread_pool_t *pool,
                            int dry_run,
                            mse_all_printings_cards_t *cards);
