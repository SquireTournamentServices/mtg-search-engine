#pragma once
#include "./interpretor.h"
#include "./search.h"

/// An entry to the builder stack, this is either a node or the open bracket marker
/// with the add node method this allows to close bracket to pop until it hits the
/// start of the statement. Finalise is used to clear the stack aferwarsds.
typedef struct mse_query_builder_stack_entry_t {
    mse_interp_node_t *node;
    int is_open_bracket;
} mse_query_builder_stack_entry_t;

/// The query builder builds a stack of operations and, stores the output tree in root,
/// you need to finalise the tree to pop anything that is left on the stack (which is
/// likely non-empty). The shunting yard algorithm is used to build the output tree
typedef struct mse_query_builder_t {
    size_t stack_len, stack_ptr;
    mse_query_builder_stack_entry_t *stack;
    mse_interp_node_t *root, *current;
} mse_query_builder_t;

/// Inits the builder
int mse_init_query_builder(mse_query_builder_t *builder);

/// Adds the node to the query builder
int mse_query_builder_add_node(mse_query_builder_t *builder, mse_interp_node_t *node);

/// Called to enter a statement (a bracketed region of a query) call on open bracket in the parser
int mse_query_builder_enter_statement(mse_query_builder_t *builder);

/// Called to exit a statement (a bracketed region of a query) call on close bracket in the parser
int mse_query_builder_exit_statement(mse_query_builder_t *builder);

/// Turns the query into a tree which is placed in ret, the query builder is always
/// freed by this function. Even no fail you need to free ret with the right function.
int mse_finalise_query_builder(mse_query_builder_t *builder, mse_interp_node_t **ret);
